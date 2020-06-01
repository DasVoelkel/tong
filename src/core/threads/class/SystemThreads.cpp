#include "core/threads/class/SystemThreads.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

bool SystemThread::lib_inited_ = false;
ALLEGRO_FONT* SystemThread::default_font_ = NULL;

SystemThread::SystemThread(std::string name, THREAD_TYPES type, SystemThread *parent) : name_(name), type_(type), parent_(parent), waiting_for_me_(0) {
  fprintf(stderr, "Building Thread: %s, %s , %s \n", name_.c_str(), repr(type_), parent_ ? "parent" : "no parent");
  
  if (!lib_inited_) {
    fprintf(stderr, "inting lib first time\n");
    
    must_init(al_init(), "allegro");
    must_init(al_install_mouse(), "al_install_mouse");
    must_init(al_install_keyboard(), "keyboard");
    
    must_init(al_init_image_addon(), "al_init_image_addon");
    must_init(al_init_primitives_addon(), "al_init_primitives_addon");
    must_init(al_init_font_addon(), "al_init_font_addon");
    must_init(al_init_ttf_addon(), "al_init_ttf_addon");
  
    default_font_ = al_create_builtin_font();
    must_init(default_font_, "default font");
    lib_inited_ = true;
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
  
  
  thread_state_ = THREAD_STATES::T_STOPPED;
  fprintf(stderr, "Built Thread: %s \n", name_.c_str());
}

SystemThread::~SystemThread() {
  fprintf(stderr, "destroying thread %s state %s waiting %d \n", name_.c_str(), repr(thread_state_), waiting_for_me_.load());
  
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
      if (get_state_() == THREAD_STATES::T_DELETED) {
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
  fprintf(stderr, "Thread %s changing from %s to %s waiting: %d\n", name_.c_str(), repr(thread_state_), repr(new_state), waiting_for_me_.load());
  
  ALLEGRO_EVENT tmp_event;
  tmp_event.user.type = THREAD_STATE_CHANGE_EVENT;
  thread_state_ = new_state;
  
  if (al_emit_user_event(state_change_event_source_, &tmp_event, NULL)) {
    fprintf(stderr, "Thread %s changed state to %s\n ", name_.c_str(), repr(thread_state_));
  } else {
    fprintf(stderr, "Thread %s changed state, no one cared\n ", name_.c_str(), repr(thread_state_));
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
    
    if(event.type == THREAD_CONTROL_EVENT){
      fprintf(stderr, "Thread %s handling control event \n ", name_.c_str());
      control_event_handler(event);
      continue;
    }
    
    switch (get_state_()) {
      case T_RUNNING:
        thread_retval_ = thread_(event, thread_args_);
        break;
      case T_STOPPING:
        fprintf(stderr, "Stopping thread %s1 \n", name_.c_str());
      case T_STOPPED:
        update_thread_state(T_STOPPED);
        return;
        break;
      case T_DELETED:
        assert(false && "State should either be running or stopped or stopping");
        
        break;
    }
    
    
  }
}

bool SystemThread::start(void *args) {
  switch (thread_state_) {
    
    case T_RUNNING:
      fprintf(stderr, "Already running \n ");
      return true;
      break;
    case T_STOPPING:
      fprintf(stderr, "currently in the process of stopping, waiting until done, then restarting \n ");
      wait_for_state(T_STOPPED);
      // purposefully do not break! Just run into the handler for stopped
    case T_STOPPED:
      fprintf(stderr, "Start after stop \n ");
      
      if (running_thread_) {
        al_destroy_thread(running_thread_);
        running_thread_ = NULL;
      }
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
      fprintf(stderr, "Currently stopping \n ");
      wait_for_state(THREAD_STATES::T_STOPPED);
      break;
    case T_STOPPED:
      fprintf(stderr, "Stopped \n ");
      return true;
      break;
    case T_DELETED:
      break;
  }
  
  return false;
  
}

bool SystemThread::send_control_event(ALLEGRO_EVENT &event){
  
  fprintf(stderr, "Thread %s trying to Send control event\n", name_.c_str());
  
  
  auto instance = this;
  while(instance->type_ != THREAD_TYPES::THREAD_CONTROLLER){
    if(instance->parent_)
      instance = instance->parent_;
    else{
      fprintf(stderr, "Thread %s found no controller in chain \n ", name_.c_str());
      return false;
    }
  }
  

    ALLEGRO_EVENT tmp_event = event;
    tmp_event.user.type = THREAD_CONTROL_EVENT;
    if (al_emit_user_event(instance->state_change_event_source_, &tmp_event, NULL)) {
      fprintf(stderr, "Thread %s sent control \n ", name_.c_str());
    } else {
      fprintf(stderr, "Thread %s sent control, no one cared\n ", name_.c_str());
    }
    
    return true;

  
  

  return false;
}



// util
const

char *SystemThread::repr(THREAD_STATES state) {
  
  switch (state) {
    case THREAD_STATES::T_RUNNING:
      return "--T_RUNNING--";
      break;
    case THREAD_STATES::T_STOPPING:
      return "--T_STOPPING--";
      break;
    case THREAD_STATES::T_STOPPED:
      return "--STOPPED--";
      break;
    
    case T_DELETED:
      return "--T_DELETED!--";
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

void SystemThread::print()const {
  fprintf(stderr, "Thread \"%s\" Type %s State %s processed %d \n", name_.c_str(),  repr(type_), repr(thread_state_),(int) events_processed_);
}

//ALLEGRO_FONT * SystemThread::default_font = NULL;
ALLEGRO_FONT *SystemThread::get_default_font() {
  assert(default_font_);
  return default_font_;
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

