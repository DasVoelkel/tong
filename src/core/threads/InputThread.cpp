#include "core/threads/InputThread.hpp"

InputThread::InputThread() : SystemThread(std::string("InputThread"), THREAD_TYPES::THREAD_WORKER)

{
  // create your own event sources etc here
  fprintf(stderr,"Created input thread \n");
  
  al_register_event_source(thread_event_queue_, al_get_keyboard_event_source());

  
}

// destroy everything you have created bevore to prevent mem leaks
InputThread::~InputThread() {

}



void *InputThread::thread_(ALLEGRO_EVENT &event, void *args) {
  fprintf(stderr,"InputThread thread_ run \n");
  
  return NULL;
}

void InputThread::control_event_handler(ALLEGRO_EVENT &event) {

}
