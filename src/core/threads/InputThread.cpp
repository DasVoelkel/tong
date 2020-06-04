#include <core/threads/CompositorThread.hpp>
#include "core/threads/InputThread.hpp"

InputThread::InputThread(SystemThread *parent) : SystemThread(std::string("InputThread"), THREAD_TYPES::THREAD_WORKER, parent) {
  // create your own event sources etc here
  fprintf(stderr, "Created input thread \n");
  
  
}

// destroy everything you have created bevore to prevent mem leaks
InputThread::~InputThread() {
LOG(TAG_,"Destroyed \n");
}

void InputThread::startup() {
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  
  memset(key_, 0, sizeof(key_)); // do this when restarting as well
}

void InputThread::shutdown() {

}


void *InputThread::thread_(ALLEGRO_EVENT &event, void *args) {
  
  switch (event.type) {
    case ALLEGRO_EVENT_KEY_UP:
      keyboard_update(&event);
      break;
    case ALLEGRO_EVENT_KEY_DOWN:
      keyboard_update(&event);
      break;
    case ALLEGRO_EVENT_KEY_CHAR:
      // do nothing
      break;
    
    
    default:
      fprintf(stderr, "unexpected event in %s ! >%i<\n", name_.c_str(), event.type);
      break;
  }
  
  return NULL;
}


void InputThread::control_event_handler(UserEvent &event) {

}


// keyboard processing
void InputThread::keyboard_seen() {
  for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
    key_[i] &= KEY_SEEN;
}

void InputThread::keyboard_update(ALLEGRO_EVENT *event) {
  
  switch (event->type) {
    
    case ALLEGRO_EVENT_KEY_DOWN:
      key_[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
      break;
    case ALLEGRO_EVENT_KEY_UP:
      key_[event->keyboard.keycode] &= KEY_RELEASED;
      break;
  }
  process_keys();
}

void InputThread::process_keys() {
  
  for (int processed_key_id = 0; processed_key_id < ALLEGRO_KEY_MAX; processed_key_id++) {
    
    if (key_[processed_key_id]) {
      switch (processed_key_id) {
        case ALLEGRO_KEY_ESCAPE:
          // exit program
          //request_thread_state_change(EXIT);
          close();
          LOG(TAG_, "Escape key\n");
          break;
        
        case ALLEGRO_KEY_R:
          // restart display thread
          //request_thread_state_change(RESTART);
          restart();
          LOG(TAG_, "R key\n");
          
          break;
        
        default:
          break;
      }
      //fprintf(stderr, "Processed: %s \n", al_keycode_to_name(processed_key_id));
    }
    
    key_[processed_key_id] &= KEY_SEEN;
  }
}

void InputThread::draw() {

}


