#include "core/threads/class/SystemThreads.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

bool SystemThread::lib_inited = false;

SystemThread::SystemThread(std::string name, THREAD_TYPES type, SystemThread *parent) : name_(name), type_(type), parent_(parent), waiting_for_me_(0) {
  fprintf(stderr, "Building Thread: %s, %s , %s \n", name_.c_str(), repr(type_), parent_ ? "parent" : "no parent");
  
  if (!lib_inited) {
    fprintf(stderr, "inting lib first time\n");
    
    must_init(al_init(), "allegro");
    must_init(al_install_mouse(), "al_install_mouse");
    must_init(al_install_keyboard(), "keyboard");
    
    must_init(al_init_image_addon(), "al_init_image_addon");
    must_init(al_init_primitives_addon(), "al_init_primitives_addon");
    must_init(al_init_font_addon(), "al_init_font_addon");
    must_init(al_init_ttf_addon(), "al_init_ttf_addon");
    
    lib_inited = true;
  }
  
  if (type_ == THREAD_TYPES::THREAD_CONTROLLER) {
    control_event_source_ = new ALLEGRO_EVENT_SOURCE;
    al_init_user_event_source(control_event_source_);
  }
  state_change_event_source_ = new ALLEGRO_EVENT_SOURCE;
  al_init_user_event_source(state_change_event_source_);
  
  thread_event_queue_ = al_create_event_queue();
  must_init(thread_event_queue_, "SystemThread::thread_event_queue_");
  
  
  al_register_event_source(thread_event_queue_, state_change_event_source_);
  
  {
// add parenting control sources
    SystemThread *next = parent_;
    while (next) {
      if (next->get_type_() == THREAD_TYPES::THREAD_CONTROLLER) {
        assert(next->control_event_source_ && "Control Thread without Control source... ?");
        al_register_event_source(thread_event_queue_, next->control_event_source_);
      }
    }
  }
  
  
  state_ = THREAD_STATES::STOPPED;
  fprintf(stderr, "Built Thread: %s \n", name_.c_str());
}

SystemThread::~SystemThread() {
  fprintf(stderr, "destroying thread %s state %s waiting %d \n", name_.c_str(), repr(state_), waiting_for_me_.load());
  
  assert(state_ == THREAD_STATES::STOPPED && "Thread not done, shouldn't stop");
  while (waiting_for_me_.load() != 0) {
    update_thread_state(THREAD_STATES::DELETED);
  }
  al_destroy_user_event_source(state_change_event_source_);
  
  delete state_change_event_source_;
  if (control_event_source_) {
    al_destroy_user_event_source(control_event_source_);
    delete control_event_source_;
  }
  
  al_destroy_event_queue(thread_event_queue_);
  
  fprintf(stderr, "destroyed %s \n", name_.c_str());
}


bool SystemThread::wait_for_state(THREAD_STATES expected, bool blocking) {
  fprintf(stderr, "Waiting for state %s blocking: %d in thread %s  \n", repr(expected), blocking, name_.c_str());
  
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
      if (get_state_() == THREAD_STATES::DELETED) {
        waiting_for_me_--;
        al_unregister_event_source(tmp_queue, state_change_event_source_);
        fprintf(stderr, "Thread was deleted while some other thread was waiting? \n");
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
  fprintf(stderr, "Thread %s changing from %s to %s waiting: %d\n", name_.c_str(), repr(state_), repr(new_state), waiting_for_me_.load());
  
  ALLEGRO_EVENT tmp_event;
  tmp_event.user.type = THREAD_STATE_CHANGE_EVENT;
  state_ = new_state;
  
  if (al_emit_user_event(state_change_event_source_, &tmp_event, NULL)) {
    fprintf(stderr, "Thread %s changed state to %s\n ", name_.c_str(), repr(state_));
  } else {
    fprintf(stderr, "Thread %s changed state, no one cared\n ", name_.c_str(), repr(state_));
  }
}

// core
// make a wrapper so that the actual code only gets the event itself without handling the rest

// make a wrapper so that the actual code only gets the event itself without handling the rest
void *SystemThread::thread_wrapper(ALLEGRO_THREAD *thr, void *sysT) {
  //fprintf(stderr, "thread wrapper accessed \n");
  auto instance = (SystemThread *) sysT;
  instance->thread_event_filter_();
  return NULL;
}

void SystemThread::thread_event_filter_() {
  while (true) {
    ALLEGRO_EVENT event;
    al_wait_for_event(thread_event_queue_, &event);
    events_processed_++;
    
    switch (get_state_()) {
      case RUNNING:
        thread_retval_ = thread_(event, thread_args_);
        break;
      case STOPPING:
        fprintf(stderr, "Stopping thread %s1 \n", name_.c_str());
      case STOPPED:
        update_thread_state(STOPPED);
        return;
        break;
      case DELETED:
        assert(false && "State should either be running or stopped or stopping");
        
        break;
    }
    
    
  }
}

bool SystemThread::start(void *args) {
  switch (state_) {
    
    case RUNNING:
      fprintf(stderr, "Already running \n ");
      return true;
      break;
    case STOPPING:
      fprintf(stderr, "currently in the process of stopping, waiting until done, then restarting \n ");
      wait_for_state(STOPPED);
      // purposefully do not break! Just run into the handler for stopped
    case STOPPED:
      fprintf(stderr, "Start after stop \n ");
      
      if (running_thread_) {
        al_destroy_thread(running_thread_);
        running_thread_ = NULL;
      }
      thread_args_ = args;
      running_thread_ = al_create_thread(thread_wrapper, this);
      al_start_thread(running_thread_);
      
      update_thread_state(RUNNING);
      break;
    case DELETED:
      break;
  }
  return false;
}

bool SystemThread::stop() {
  switch (state_) {
    case RUNNING:
      update_thread_state(THREAD_STATES::STOPPING);
      return true;
      break;
    case STOPPING:
      fprintf(stderr, "Currently stopping \n ");
      wait_for_state(THREAD_STATES::STOPPED);
      break;
    case STOPPED:
      fprintf(stderr, "Stopped \n ");
      return true;
      break;
    case DELETED:
      break;
  }
  
  return false;
  
}

void SystemThread::stop_self() {
  update_thread_state(THREAD_STATES::STOPPING);
}

// util
const

char *SystemThread::repr(THREAD_STATES state) {
  
  switch (state) {
    case THREAD_STATES::RUNNING:
      return "--RUNNING--";
      break;
    case THREAD_STATES::STOPPING:
      return "--STOPPING--";
      break;
    case THREAD_STATES::STOPPED:
      return "--STOPPED--";
      break;
    
    case DELETED:
      return "--DELETED!--";
      break;
    default:
      return "--UNKNOWN--";
      break;
    
  }
}

const char *SystemThread::repr(THREAD_TYPES type) {
  switch (type) {
    
    case THREAD_TYPES::THREAD_NOTYPE:
      return "-NOTYPE-";
      break;
    case THREAD_TYPES::THREAD_WORKER:
      return "-WORKER-";
      break;
    case THREAD_TYPES::THREAD_CONTROLLER:
      return "-CONTROLLER-";
      break;
  }
  return "?";
}

void SystemThread::must_init(bool test, const char *description) {
  if (test)
    return;
  
  fprintf(stderr, "couldn't initialize %s\n", description);
  exit(1);
}

const void SystemThread::print() {
  fprintf(stderr, "Thread \"%s\" Type %s State %s processed %d \n", name_.c_str(), repr(state_), repr(type_), (int) events_processed_);
}