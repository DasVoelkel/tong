#include "core/threads/class/SystemThreads.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <algorithm>
#include <allegro5/allegro_color.h>

const int  DISP_W =  640;
const int  DISP_H =  480;
const float FPS = 1.0;
const char* CLEAR_TO_COLOR = "white";

ALLEGRO_FONT *SystemThread::default_font_ = NULL;
ALLEGRO_DISPLAY *SystemThread::display_ = NULL;
ALLEGRO_TIMER *SystemThread::fps_timer_ = NULL;
size_t SystemThread::obj_counter_ = 0;
ALLEGRO_MUTEX *SystemThread::object_counter_lock_ = al_create_mutex();
std::vector<ALLEGRO_BITMAP *> SystemThread::bitmaps_ = {};


SystemThread::SystemThread(std::string name, THREAD_TYPES type, SystemThread *parent) : name_(name), type_(type), parent_(parent), waiting_for_me_(0) {
  TAG_ = name_.c_str();
  
  LOG(TAG_, "Creating: %s , %s \n", repr(type_), parent_ ? "parent" : "no parent");
  al_lock_mutex(object_counter_lock_);
  if (obj_counter_ == 0) {
    must_init(al_init(), "allegro");
    must_init(al_install_mouse(), "al_install_mouse");
    must_init(al_install_keyboard(), "al_install_keyboard");
    must_init(al_init_image_addon(), "al_init_image_addon");
    must_init(al_init_primitives_addon(), "al_init_primitives_addon");
    must_init(al_init_font_addon(), "al_init_font_addon");
    must_init(al_init_ttf_addon(), "al_init_ttf_addon");
    
    default_font_ = al_create_builtin_font();
    must_init(default_font_, "default font");
    
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    
    
    display_ = al_create_display(DISP_W, DISP_H);
    must_init(display_, "display");
    
    fps_timer_ = al_create_timer(1.0 / FPS);
    must_init(fps_timer_, "draw-thread-timer");
    
    al_start_timer(fps_timer_);
  }
  
  
  thread_event_queue_ = al_create_event_queue();
  
  al_register_event_source(thread_event_queue_, al_get_display_event_source(display_));
  al_register_event_source(thread_event_queue_, al_get_timer_event_source(fps_timer_));
  
  
  if (type_ == THREAD_TYPES::THREAD_CONTROLLER) {
    control_event_source_ = new ALLEGRO_EVENT_SOURCE;
    al_init_user_event_source(control_event_source_);
    al_register_event_source(thread_event_queue_, control_event_source_);
    
  }
  state_change_event_source_ = new ALLEGRO_EVENT_SOURCE;
  al_init_user_event_source(state_change_event_source_);
  
  must_init(thread_event_queue_, "SystemThread::thread_event_queue_");
  al_register_event_source(thread_event_queue_, state_change_event_source_);
  
  {
// add parenting control sources
    SystemThread *next = parent_;
    while (next) {
      if (next->get_type_() == THREAD_TYPES::THREAD_CONTROLLER) {
        assert(next->control_event_source_ && "Control Thread without Control source... ?");
        LOG(TAG_, "subscribed to, %s @ %p \n", next->name_.c_str(), next->control_event_source_);
        
        al_register_event_source(thread_event_queue_, next->control_event_source_);
      }
      next = next->parent_;
    }
  }
  
  
  thread_state_ = THREAD_STATES::T_STOPPED;
  obj_counter_++;
  al_unlock_mutex(object_counter_lock_);
  
}

SystemThread::~SystemThread() {
  
  
  assert(thread_state_ == THREAD_STATES::T_STOPPED && "Thread not done, shouldn't stop");
  while (waiting_for_me_.load() != 0) {
    update_thread_state(THREAD_STATES::T_DELETED);
  }
  al_destroy_user_event_source(state_change_event_source_);
  delete state_change_event_source_;
  
  
  if (control_event_source_) {
    al_destroy_user_event_source(control_event_source_);
    delete control_event_source_;
  }
  
  al_destroy_event_queue(thread_event_queue_);
  if (internal_bitmap_) {
    al_destroy_bitmap(internal_bitmap_);
    internal_bitmap_ = NULL;
  }
  
  if (internal_bitmap_lock_) {
    al_destroy_mutex(internal_bitmap_lock_);
    internal_bitmap_lock_ = NULL;
  }
  
  al_lock_mutex(object_counter_lock_);
  obj_counter_--;
  if (obj_counter_ == 0) {
    if (display_)
      al_destroy_display(display_);
    
    if (fps_timer_)
      al_destroy_timer(fps_timer_);
  }
  
  
}


bool SystemThread::wait_for_state(THREAD_STATES expected, bool blocking) {
  LOG(TAG_, "Wait %s -> %s\n", repr(get_state_()), repr(expected));
  if (!blocking) {
    return get_state_() == expected;
  }
  
  ALLEGRO_EVENT_QUEUE *tmp_queue = al_create_event_queue();
  al_register_event_source(tmp_queue, state_change_event_source_);
  waiting_for_me_++;
  
  {
    ALLEGRO_EVENT buffer;
    
    while (get_state_() != expected) {
      al_wait_for_event(tmp_queue, &buffer);
      if (get_state_() == THREAD_STATES::T_DELETED) {
        waiting_for_me_--;
        al_unregister_event_source(tmp_queue, state_change_event_source_);
        LOG(TAG_, "Was deleted while someone was waiting\n");
        al_destroy_event_queue(tmp_queue);
        return false;
        
      }
    }
  }
  waiting_for_me_--;
  al_unregister_event_source(tmp_queue, state_change_event_source_);
  al_destroy_event_queue(tmp_queue);
  
  return true;
}

void SystemThread::update_thread_state(THREAD_STATES new_state) {
  LOG(TAG_, "State: %s -> %s\n", repr(get_state_()), repr(new_state));
  ALLEGRO_EVENT tmp_event;
  tmp_event.user.type = THREAD_STATE_CHANGE_EVENT;
  thread_state_ = new_state;
  
  al_emit_user_event(state_change_event_source_, &tmp_event, NULL);
  
}

// core
// make a wrapper so that the actual code only gets the event itself without handling the rest

// make a wrapper so that the actual code only gets the event itself without handling the rest
void *SystemThread::thread_wrapper(ALLEGRO_THREAD *thr, void *sysT) {
  auto instance = (SystemThread *) sysT;
  instance->thread_event_filter_();
  return NULL;
}

void SystemThread::thread_event_filter_() {
  while (true) {
    assert(get_state_() != T_DELETED);
    
    if (redraw_needed_ && al_event_queue_is_empty(thread_event_queue_)) {
      thread_draw_wrapper();
      redraw_needed_ = false;
      if (draw_to_display_) {
        al_set_target_backbuffer(display_);
        al_clear_to_color(al_color_name(CLEAR_TO_COLOR));
        draw_internal_bitmap();
        al_flip_display();
        al_set_target_bitmap(NULL);
      }
    }
    
    ALLEGRO_EVENT event;
    al_wait_for_event(thread_event_queue_, &event);
    events_processed_++;
    
    if (event.type == THREAD_CONTROL_EVENT) {
      control_event_handler(event.user.data1);
    }
    
    if(get_state_() == T_STOPPED or get_state_() == T_STOPPING){
      update_thread_state(T_STOPPED);
      return;
    }
    
    // we are "running"
    switch (event.type) { // filter special things out
      case (THREAD_CONTROL_EVENT): // already processed
        continue;
        break;
      case (THREAD_STATE_CHANGE_EVENT): // not relevant for normal procedure
        continue;
        break;
      case ALLEGRO_EVENT_TIMER:     // only on fps required
        if (event.timer.source == fps_timer_) {
          if (is_fps_rendering_)
            redraw_needed_ = true;
          continue;
        }
        break;
      case ALLEGRO_EVENT_DISPLAY_RESIZE: {
        LOG(TAG_,"ALLEGRO_EVENT_DISPLAY_RESIZE\n");
  
        if(al_get_display_width(display_) != event.display.width or al_get_display_height(display_) != event.display.height ){
          al_resize_display(display_,event.display.width,event.display.height);
        }
        al_acknowledge_resize(display_);
        if (internal_bitmap_)
          adjust_bitmap_size();
      }
        break;
    }
    thread_retval_ = thread_(event, thread_args_);
  
  
  }
}

bool SystemThread::start(void *args) {
  al_flush_event_queue(thread_event_queue_);
  switch (thread_state_) {
    
    case T_RUNNING:
      return true;
      break;
    case T_STOPPING:
      wait_for_state(T_STOPPED);
      // purposefully do not break! Just run into the handler for stopped
    case T_STOPPED:
      if (running_thread_) {
        al_join_thread(running_thread_, NULL);
        al_destroy_thread(running_thread_);
        
        running_thread_ = NULL;
      }
      if (args)
        thread_args_ = args;
      running_thread_ = al_create_thread(thread_wrapper, this);
      al_start_thread(running_thread_);
      
      update_thread_state(T_RUNNING);
      break;
    case T_DELETED:
      break;
  }
  return false;
}

bool SystemThread::stop() {
  
  
  switch (thread_state_) {
    case T_RUNNING:
      update_thread_state(THREAD_STATES::T_STOPPING);
      return true;
      break;
    case T_STOPPING:
      wait_for_state(THREAD_STATES::T_STOPPED);
      break;
    case T_STOPPED:
      return true;
      break;
    case T_DELETED:
      break;
  }
  
  return false;
  
}

bool SystemThread::send_control_event(const size_t control_event) {
  
  
  auto instance = this;
  while (instance->type_ != THREAD_TYPES::THREAD_CONTROLLER) {
    if (instance->parent_)
      instance = instance->parent_;
    else {
      assert(false);
      return false;
    }
  }
  
  
  ALLEGRO_EVENT tmp_event;
  tmp_event.user.type = THREAD_CONTROL_EVENT;
  tmp_event.user.data1 = control_event;
  
  al_emit_user_event(instance->control_event_source_, &tmp_event, NULL);
  //LOG(TAG_,"Sent Control event type: %d @ %p \n",tmp_event.user.data1,instance->control_event_source_);
  
  return true;
}


// util
const

char *SystemThread::repr(THREAD_STATES state) {
  
  switch (state) {
    case THREAD_STATES::T_RUNNING:
      return "T_RUNNING";
      break;
    case THREAD_STATES::T_STOPPING:
      return "T_STOPPING";
      break;
    case THREAD_STATES::T_STOPPED:
      return "T_STOPPED";
      break;
    case T_DELETED:
      return "T_DELETED!";
      break;
    default:
      return "T_UNKNOWN";
      break;
    
  }
}

const char *SystemThread::repr(THREAD_TYPES type) {
  switch (type) {
    
    case THREAD_TYPES::THREAD_NOTYPE:
      return "NOTYPE";
      break;
    case THREAD_TYPES::THREAD_WORKER:
      return "WORKER";
      break;
    case THREAD_TYPES::THREAD_CONTROLLER:
      return "CONTROLLER";
      break;
  }
  return "?";
}

void SystemThread::must_init(bool test, const char *description) {
  if (test)
    return;
  LOG("SystemThread", "Couldn't init %s \n", description);
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

void SystemThread::create_internal_bitmap() {
  
  bool unlock = false;
  
  if (internal_bitmap_lock_) {
    al_lock_mutex(internal_bitmap_lock_);
    unlock = true;
  } else {
    internal_bitmap_lock_ = al_create_mutex();
  }
  
  if (internal_bitmap_) {
    al_destroy_bitmap(internal_bitmap_);
    
    auto found = std::find(bitmaps_.begin(), bitmaps_.end(), internal_bitmap_);
    assert(found != bitmaps_.end());
    bitmaps_.erase(found);
  }
  
  internal_bitmap_ = al_create_bitmap(al_get_display_width(display_), al_get_display_height(display_));
  bitmaps_.push_back(internal_bitmap_);
  
  if (unlock)
    al_unlock_mutex(internal_bitmap_lock_);
}


void SystemThread::adjust_bitmap_size() {
  
  
  al_lock_mutex(internal_bitmap_lock_);
  al_destroy_bitmap(internal_bitmap_);
  LOG(TAG_,"Adjusted H:%d W:%d -> H:%d W:%d \n",al_get_bitmap_height(internal_bitmap_),al_get_bitmap_width(internal_bitmap_),al_get_display_height(display_), al_get_display_width(display_));
  internal_bitmap_ = al_create_bitmap(al_get_display_width(display_), al_get_display_height(display_));
  
  al_unlock_mutex(internal_bitmap_lock_);
  
  
}

void *SystemThread::thread_draw_wrapper() {
  assert(internal_bitmap_ && "redraw triggered but the Thread has no internal bitmaps!");
  
  al_lock_mutex(internal_bitmap_lock_);
  al_set_target_bitmap(internal_bitmap_);
  
  al_clear_to_color(al_color_name(CLEAR_TO_COLOR));
  times_drawn_++;
  draw();
  
  al_set_target_bitmap(NULL);
  al_unlock_mutex(internal_bitmap_lock_);
  return nullptr;
}

void SystemThread::draw_internal_bitmap() {
  
  al_lock_mutex(internal_bitmap_lock_);
  assert(al_get_target_bitmap() && "Shouldn't be null");
  al_draw_bitmap(internal_bitmap_, 0, 0, 0);
  al_unlock_mutex(internal_bitmap_lock_);
}

void SystemThread::enable_fps_rendering(bool to_display) {
  if (!internal_bitmap_)
    create_internal_bitmap();
  al_register_event_source(thread_event_queue_, al_get_timer_event_source(fps_timer_));
  is_fps_rendering_ = true;
  draw_to_display_ = to_display;
}

/*TEMPLATE DERIVATIVE _NAME_

 .hpp
 
 #include <core/threads/class/SystemThreads.hpp>

 class _NAME_ : public SystemThread {

private:
// stuff only this class should know
public:
  // stuff everyone can access
public:
  _NAME_();
  
  ~_NAME_();
  
  // only absolutely neccessary override
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  virtual void control_event_handler(ALLEGRO_EVENT & event) override;


};
 
 
 
 .cpp
 #include "core/threads/_NAME_.hpp"

_NAME_::_NAME_() : SystemThread(std::string("_NAME_"), type)

{
  // create your own event sources etc here
  
  
  // start up here
  

  
}

 // destroy everything you have created bevore to prevent mem leaks
_NAME_::~_NAME_() {

}



void *_NAME_::thread_(ALLEGRO_EVENT &event, void *args) {

return NULL;
}
  void InputThread::control_event_handler(ALLEGRO_EVENT &event) {
  
}
* */

