#include "core/threads/class/SystemThreads.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <algorithm>
#include <utility>
#include <allegro5/allegro_color.h>

const int DISP_W = 640;
const int DISP_H = 480;
const float FPS = 1.0;
const char *CLEAR_TO_COLOR = "white";


ALLEGRO_FONT *SystemThread::default_font_ = NULL;
ALLEGRO_TIMER *SystemThread::fps_timer_ = NULL;

size_t SystemThread::obj_counter_ = 0;
ALLEGRO_MUTEX *SystemThread::object_counter_lock_ = al_create_mutex();

ALLEGRO_EVENT_SOURCE *SystemThread::global_source_ = NULL;

// it's static but not shared or protected, just the  measurements are
ALLEGRO_DISPLAY *SystemThread::display_ = NULL;


SystemThread::SystemThread(std::string name, THREAD_TYPES type, SystemThread *parent) : name_(std::move(name)), type_(type), parent_(parent), waiting_for_me_(0), TAG_(name_.c_str()) {
  
  // control that we always have knowledge of our static stuff
  al_lock_mutex(object_counter_lock_);
  
  if (obj_counter_ == 0) {
    must_init(al_init(), "allegro");
    must_init(al_install_mouse(), "al_install_mouse");
    must_init(al_install_keyboard(), "al_install_keyboard");
    must_init(al_init_image_addon(), "al_init_image_addon");
    must_init(al_init_primitives_addon(), "al_init_primitives_addon");
    must_init(al_init_font_addon(), "al_init_font_addon");
    must_init(al_init_ttf_addon(), "al_init_ttf_addon");
    
    global_source_ = new ALLEGRO_EVENT_SOURCE; // global event source
    al_init_user_event_source(global_source_);
    
    default_font_ = al_create_builtin_font(); // needed font
    must_init(default_font_, "default font");
    
    fps_timer_ = al_create_timer(1.0 / FPS); // trigger for redrawing
    must_init(fps_timer_, "draw-thread-timer");
    
    al_start_timer(fps_timer_); // start now, who cares
  }
  
  event_queue = al_create_event_queue();
  
  // global source
  al_register_event_source(event_queue, global_source_); // everyone will know the global event source
  
  // local source
  local_event_source_ = new ALLEGRO_EVENT_SOURCE; // local event source
  al_init_user_event_source(local_event_source_);
  must_init(local_event_source_, "SystemThread::thread_event_queue_");
  al_register_event_source(event_queue, local_event_source_);
  
  state_change_source = new ALLEGRO_EVENT_SOURCE;
  al_init_user_event_source(state_change_source);
  must_init(state_change_source, "SystemThread::state_change_source");
  
  
  // If you are a Controller, create a chain underneath you
  if (type_ == THREAD_TYPES::THREAD_CONTROLLER) {
    chain_event_source_ = new ALLEGRO_EVENT_SOURCE;
    al_init_user_event_source(chain_event_source_);
    //al_register_event_source(event_queue, chain_event_source_); // TODO test if this makes more sense, to not sub to yourself
  }
  
  // chain registering, add all parents chain sources
  {
    SystemThread *next = parent_;
    while (next) {
      if (next->get_type_() == THREAD_TYPES::THREAD_CONTROLLER) {
        assert(next->chain_event_source_ && "Control Thread without Control source... ?");
        LOG(TAG_, "subscribed to, %s @ %p \n", next->name_.c_str(), next->chain_event_source_);
        al_register_event_source(event_queue, next->chain_event_source_);
      }
      next = next->parent_; // up the chain to the next parent
    }
  }
  
  running_thread_ = al_create_thread(thread_wrapper, this);
  
  LOG(TAG_, "Created: %s , %s \n", repr(type_), parent_ ? "parent" : "no parent");
  obj_counter_++;
  
  al_unlock_mutex(object_counter_lock_);
  
}

SystemThread::~SystemThread() {
  
  assert(thread_state_ == THREAD_STATES::T_STOPPED && "Destroying Running object");
  
  while (waiting_for_me_.load() != 0) {
    dispatch_event<LOCAL_THREAD_CMD>(SYSTEMTHREAD_EVENTS::THREAD_EVENT_LOCAL, LOCAL_THREAD_CMD::L_DELETING);
  }
  
  al_destroy_user_event_source(local_event_source_);
  delete local_event_source_;
  
  al_destroy_user_event_source(state_change_source);
  delete state_change_source;
  
  if (chain_event_source_) {
    al_destroy_user_event_source(chain_event_source_);
    delete chain_event_source_;
  }
  
  al_destroy_event_queue(event_queue);
  
  al_lock_mutex(object_counter_lock_);
  obj_counter_--;
  
  if (obj_counter_ == 0) {
    if (fps_timer_)
      al_destroy_timer(fps_timer_);
  }
  if (!parent_) {
    al_destroy_display(display_);
    display_ = NULL;
  }
  
  al_unlock_mutex(object_counter_lock_);
  
  
}


/*   _                   _ _ _
    (_)                 | | (_)
 ___ _  __ _ _ __   __ _| | |_ _ __   __ _
/ __| |/ _` | '_ \ / _` | | | | '_ \ / _` |
\__ \ | (_| | | | | (_| | | | | | | | (_| |
|___/_|\__, |_| |_|\__,_|_|_|_|_| |_|\__, |
        __/ |                         __/ |
       |___/                         |___/
 * */
template<class T>
void SystemThread::dispatch_event(SYSTEMTHREAD_EVENTS type, T event) {
  
  ALLEGRO_EVENT tmp_ev;
  UserEvent *tmp_ue = new UserEvent;
  tmp_ue->event_type = type;
  tmp_ue->event = (size_t) event;
  
  tmp_ev.user.type = (size_t) type;
  tmp_ev.user.data1 = (intptr_t) tmp_ue;
  
  
  switch (type) {
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_GLOBAL:
      al_emit_user_event(global_source_, &tmp_ev, UserEventDtor);
      break;
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_CHAIN:
      al_emit_user_event(chain_event_source_, &tmp_ev, UserEventDtor);
      break;
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_LOCAL:
      al_emit_user_event(local_event_source_, &tmp_ev, UserEventDtor);
      break;
  }
}


void SystemThread::change_state(THREAD_STATES new_state) {
  LOG(TAG_, "State: %s -> %s \n", repr(thread_state_), repr(new_state));
  thread_state_ = new_state;
  ALLEGRO_EVENT tmp;
  tmp.type = (size_t) SYSTEMTHREAD_EVENTS::THREAD_EVENT_STATE_CHANGE;
  al_emit_user_event(state_change_source, &tmp, NULL);
}


bool SystemThread::wait_for_state(THREAD_STATES expected, bool blocking) {
  
  if (!blocking) {
    return get_state_() == expected;
  }
  
  ALLEGRO_EVENT_QUEUE *tmp_queue = al_create_event_queue();
  
  al_register_event_source(tmp_queue, state_change_source);
  waiting_for_me_++;
  LOG(TAG_, "Wait: %s -> %s\n", repr(get_state_()), repr(expected));
  
  {
    while (true) {
      if (get_state_() == expected) {
        break;
      } else {
        ALLEGRO_EVENT buffer;
        al_wait_for_event(tmp_queue, &buffer);
        LOG(TAG_, "got new state change %s \n", repr(get_state_()));
        if (get_state_() == THREAD_STATES::T_DELETED) {
          waiting_for_me_--;
          al_unregister_event_source(tmp_queue, local_event_source_);
          LOG(TAG_, "Was deleted while someone was waiting\n");
          al_destroy_event_queue(tmp_queue);
          return false;
        }
      }
      
      
    }
  }
  
  waiting_for_me_--;
  al_unregister_event_source(tmp_queue, local_event_source_);
  al_destroy_event_queue(tmp_queue);
  LOG(TAG_, "Done waiting \n");
  
  return true;
}

/*
  ___ ___  _ __ ___
 / __/ _ \| '__/ _ \
| (_| (_) | | |  __/
 \___\___/|_|  \___|
 */

void *SystemThread::thread_wrapper(ALLEGRO_THREAD *thr, void *sysT) {
  auto instance = (SystemThread *) sysT;
  instance->thread_event_filter_();
  return NULL;
}

void SystemThread::create_root_display() {
  if (parent_)
    return;
  
  if (display_) {
    al_unregister_event_source(event_queue, al_get_display_event_source(display_));
    al_destroy_display(display_);
  }
  
  
  al_set_new_display_flags(ALLEGRO_RESIZABLE);
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  display_ = al_create_display(DISP_W, DISP_H);
  must_init(display_, "display");
  al_register_event_source(event_queue, al_get_display_event_source(display_));
  al_reset_new_display_options();
  
  LOG(TAG_, "Root Display @ %p\n", display_);
  
}

void SystemThread::thread_event_filter_() {
  
  create_root_display();
  
  while (true) {
    assert(get_state_() != THREAD_STATES::T_DELETED);
    
    if (redraw_needed_ && al_event_queue_is_empty(event_queue)) { // if we need to draw wait for it all to be chill
      thread_draw_wrapper();
      redraw_needed_ = false;
      if (!parent_) {
        al_set_target_backbuffer(display_);
        draw_internal_bitmap();
        al_flip_display();
        al_set_target_bitmap(NULL);
      }
    }
    
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);
    assert(get_state_() != THREAD_STATES::T_DELETED); // if we got deleted we are fucked either way
    
    // processed++
    
    switch (event.type) {
      case (size_t) SYSTEMTHREAD_EVENTS::THREAD_EVENT_GLOBAL:// global event handling
      {
        auto cmd_buffer = (GLOBAL_THREAD_CMD) static_cast<UserEvent *>((void *) event.user.data1)->event;
        LOG(TAG_, "%s\n", repr(cmd_buffer));
        switch (cmd_buffer) {
          case GLOBAL_THREAD_CMD::G_EXIT :
            stop(); // pass to local handler
            break;
          case GLOBAL_THREAD_CMD::G_RESTART :
            // Only the root will be able to process restarts
            if (!parent_) {
              LOG(TAG_, "Stopping...\n");
              shutdown();
              LOG(TAG_, "Restarting...\n");
              create_root_display();
              startup();
              LOG(TAG_, "Restarted \n");
            }
            break;
          default:
            assert(false && " Unkown global cmd");
        }
        al_unref_user_event(&event.user);
        continue;
        break;
      }
      case (size_t) SYSTEMTHREAD_EVENTS::THREAD_EVENT_CHAIN:                                               // chain event handling
      {
        auto cmd_buffer = (CHAIN_THREAD_CMD) static_cast<UserEvent *>((void *) event.user.data1)->event;
        LOG(TAG_, "%s\n", repr(cmd_buffer));
        switch (cmd_buffer) {
          case CHAIN_THREAD_CMD::C_EXIT:
            stop(); // just reach to the local handler so it's consistent
            break;
        }
        al_unref_user_event(&event.user);
        continue;
        break;
      }
      case (size_t) SYSTEMTHREAD_EVENTS::THREAD_EVENT_LOCAL:                                                        // local event handling
      {
        auto cmd_buffer = (LOCAL_THREAD_CMD) static_cast<UserEvent *>((void *) event.user.data1)->event;
        LOG(TAG_, "%s\n", repr(cmd_buffer));
        switch (cmd_buffer) {
          case LOCAL_THREAD_CMD::L_STOPPING:
            LOG(TAG_, "shutdown\n");
            shutdown();
            change_state(THREAD_STATES::T_STOPPED);
            LOG(TAG_, "stopped\n");
            return; // local stop
            break;
          case LOCAL_THREAD_CMD::L_STARTING:
            LOG(TAG_, "startup\n");
            startup();
            change_state(THREAD_STATES::T_RUNNING);
            break;
          case LOCAL_THREAD_CMD::L_DELETING:
            assert(false && "Do not delete a running thread \n ");
            break;
        }
        al_unref_user_event(&event.user);
        continue;
      }
      case (size_t) SYSTEMTHREAD_EVENTS::THREAD_EVENT_CUSTOM:                                                        // local event handling
      {
        control_event_handler(*(UserEvent *) event.user.data1);
        al_unref_user_event(&event.user);
        continue;
        break;
      }
    }
    
    // we are "running"
    switch (event.type) { // filter special things out
      case ALLEGRO_EVENT_TIMER:     // only on fps required
        if (event.timer.source == fps_timer_) {
          if (is_fps_enabled_) {
            redraw_needed_ = true;
          }
          continue;
        }
        break;
      case ALLEGRO_EVENT_DISPLAY_RESIZE: {
        if (!parent_) {
          if (al_get_display_width(display_) != event.display.width or al_get_display_height(display_) != event.display.height) {
            LOG(TAG_, "Resized display W: %d H: %d -> W: %d H: %d \n", al_get_display_width(display_), al_get_display_height(display_), event.display.width, event.display.height);
            al_resize_display(display_, event.display.width, event.display.height);
          }
          al_acknowledge_resize(display_);
          if (!parent_)
            adjust_bitmap_size();
          
        }
      }
      case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
        //LOG(TAG_, "ALLEGRO_EVENT_DISPLAY_SWITCH_IN\n");
        
        break;
      case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
        //LOG(TAG_, "ALLEGRO_EVENT_DISPLAY_SWITCH_OUT\n");
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        LOG(TAG_, "ALLEGRO_EVENT_DISPLAY_CLOSE\n");
        dispatch_event(SYSTEMTHREAD_EVENTS::THREAD_EVENT_GLOBAL, GLOBAL_THREAD_CMD::G_EXIT);
        continue;
        break;
    }
    thread_retval_ = thread_(event, thread_args_);
    
    
  }
}

bool SystemThread::start(void *args) {
  
  switch (thread_state_) {
    
    case THREAD_STATES::T_RUNNING:
      LOG(TAG_, "Already running\n");
      return true;
      break;
    case THREAD_STATES::T_STOPPED:
      LOG(TAG_, "Starting\n");
      if (args)
        thread_args_ = args;
      al_flush_event_queue(event_queue);
      dispatch_event(SYSTEMTHREAD_EVENTS::THREAD_EVENT_LOCAL, LOCAL_THREAD_CMD::L_STARTING);
      al_start_thread(running_thread_);
      return true;
      break;
    case THREAD_STATES::T_DELETED:
      fprintf(stderr, "Can't start deleted thread! \n");
      return false;
      break;
  }
  
  
  return true;
}

bool SystemThread::stop() {
  switch (thread_state_) {
    case THREAD_STATES::T_RUNNING:
      LOG(TAG_, "Stopping\n");
      dispatch_event(SYSTEMTHREAD_EVENTS::THREAD_EVENT_LOCAL, LOCAL_THREAD_CMD::L_STOPPING);
      return true;
      break;
    case THREAD_STATES::T_STOPPED:
      LOG(TAG_, "Already stopped\n");
      return true;
      break;
    case THREAD_STATES::T_DELETED:
      fprintf(stderr, "Can't stop deleted thread!\n");
      return false;
      break;
  }
  
  
  return true;
  
}


void SystemThread::create_internal_bitmap() {
  
  if (!internal_bitmap_) {
    internal_bitmap_ = std::make_unique<BitmapSet>();
    internal_bitmap_->mutex = al_create_mutex();
    al_lock_mutex(internal_bitmap_->mutex);
  } else {
    al_lock_mutex(internal_bitmap_->mutex);
    al_destroy_bitmap(internal_bitmap_->bitmap);
  }
  
  
  internal_bitmap_->bitmap = al_create_bitmap(al_get_display_width(display_), al_get_display_height(display_));
  
  LOG(TAG_, "Created Bitmap %p: W:%d H:%d \n", internal_bitmap_->bitmap, al_get_display_width(display_), al_get_display_height(display_));
  al_unlock_mutex(internal_bitmap_->mutex);
}


void SystemThread::adjust_bitmap_size() {
  if (!internal_bitmap_) {
    create_internal_bitmap();
    LOG(TAG_, "Adjust? Created new");
    return;
  }
  
  al_lock_mutex(internal_bitmap_->mutex);
  
  LOG(TAG_, "Adjusted W:%d H:%d -> W:%d H:%d \n", al_get_bitmap_width(internal_bitmap_->bitmap), al_get_bitmap_height(internal_bitmap_->bitmap), al_get_display_width(display_), al_get_display_height(display_));
  al_destroy_bitmap(internal_bitmap_->bitmap);
  internal_bitmap_->bitmap = al_create_bitmap(al_get_display_width(display_), al_get_display_height(display_));
  
  al_unlock_mutex(internal_bitmap_->mutex);
}

void SystemThread::thread_draw_wrapper() {
  assert(internal_bitmap_ && "redraw triggered but the Thread has no internal bitmaps!");
  
  al_lock_mutex(internal_bitmap_->mutex);
  al_set_target_bitmap(internal_bitmap_->bitmap);
  
  //LOG(TAG_, "Draw Thread bitmap %p W: %d, H: %d\n", internal_bitmap_->bitmap, al_get_bitmap_width(internal_bitmap_->bitmap), al_get_bitmap_height(internal_bitmap_->bitmap));
  
  draw();
  
  al_set_target_bitmap(NULL);
  al_unlock_mutex(internal_bitmap_->mutex);
}

void SystemThread::draw_internal_bitmap() {
  assert(al_get_target_bitmap() && "Shouldn't be null");
  
  al_lock_mutex(internal_bitmap_->mutex);
  al_draw_bitmap(internal_bitmap_->bitmap, 0, 0, 0);
  al_unlock_mutex(internal_bitmap_->mutex);
}

void SystemThread::enable_fps_rendering() {
  if (!internal_bitmap_)
    create_internal_bitmap();
  
  if (!(al_is_event_source_registered(event_queue, al_get_timer_event_source(fps_timer_)))) {
    al_register_event_source(event_queue, al_get_timer_event_source(fps_timer_));
    LOG(TAG_, "enabled FPS rendering, Timer: % s \n", al_get_timer_started(fps_timer_) ? "on" : "off");
  }
  
  is_fps_enabled_ = true;
}


void SystemThread::disable_fps_rendering() {
  is_fps_enabled_ = false;
  if ((al_is_event_source_registered(event_queue, al_get_timer_event_source(fps_timer_)))) {
    al_unregister_event_source(event_queue, al_get_timer_event_source(fps_timer_));
    LOG(TAG_, "disable FPS rendering");
    
  }
}

void SystemThread::enable_display_events() {
  if (!al_is_event_source_registered(event_queue, al_get_display_event_source(display_)))
    al_register_event_source(event_queue, al_get_display_event_source(display_));
  is_display_enabled_ = true;
  
}

void SystemThread::disable_display_events() {
  is_display_enabled_ = false;
  
  if (is_fps_enabled_)
    if (al_is_event_source_registered(event_queue, al_get_timer_event_source(fps_timer_)))
      al_unregister_event_source(event_queue, al_get_timer_event_source(fps_timer_));
}


/*       _   _ _
      | | (_) |
 _   _| |_ _| |
| | | | __| | |
| |_| | |_| | |
 \__,_|\__|_|_|*/

const char *SystemThread::repr(THREAD_STATES state) {
  
  switch (state) {
    case THREAD_STATES::T_RUNNING:
      return "T_RUNNING";
      break;
    case THREAD_STATES::T_STOPPED:
      return "T_STOPPED";
      break;
    case THREAD_STATES::T_DELETED:
      return "T_DELETED!";
      break;
    default:
      return "T_UNKNOWN";
      break;
    
  }
}

const char *SystemThread::repr(THREAD_TYPES type) {
  switch (type) {
    case THREAD_TYPES::THREAD_WORKER:
      return "WORKER";
      break;
    case THREAD_TYPES::THREAD_CONTROLLER:
      return "CONTROLLER";
      break;
  }
  return "?";
}

const char *SystemThread::repr(SYSTEMTHREAD_EVENTS event) {
  switch (event) {
    
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_GLOBAL:
      return "THREAD_EVENT_GLOBAL";
      break;
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_CHAIN:
      return "THREAD_EVENT_CHAIN";
      break;
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_LOCAL:
      return "THREAD_EVENT_LOCAL";
      break;
    case SYSTEMTHREAD_EVENTS::THREAD_EVENT_CUSTOM:
      return "THREAD_EVENT_CUSTOM";
      break;
  }
  return "?";
  
}

const char *SystemThread::repr(GLOBAL_THREAD_CMD event) {
  switch (event) {
    
    case GLOBAL_THREAD_CMD::G_EXIT:
      return "G_EXIT";
      break;
    case GLOBAL_THREAD_CMD::G_RESTART:
      return "G_RESTART";
      break;
  }
  return "?";
  
}

const char *SystemThread::repr(CHAIN_THREAD_CMD event) {
  switch (event) {
    
    case CHAIN_THREAD_CMD::C_EXIT:
      return "C_EXIT";
      break;
  }
  return "?";
  
}

const char *SystemThread::repr(LOCAL_THREAD_CMD event) {
  switch (event) {
    
    case LOCAL_THREAD_CMD::L_STOPPING:
      return "L_STOPPING";
      break;
    case LOCAL_THREAD_CMD::L_STARTING:
      return "L_STARTING";
      break;
    case LOCAL_THREAD_CMD::L_DELETING:
      return "L_DELETING";
      break;
  }
  return "?";
  
}


void SystemThread::must_init(bool test, const char *description) {
  if (test)
    return;
  LOG("SystemThread", "Couldn't startup %s \n", description);
  exit(99999);
}

void SystemThread::print() const {
  LOG(TAG_, "Type: %s, State: %s\n", repr(type_), repr(thread_state_));
}

//ALLEGRO_FONT * SystemThread::default_font = NULL;
ALLEGRO_FONT *SystemThread::get_default_font() {
  assert(default_font_);
  return default_font_;
}







