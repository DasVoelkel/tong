#include <core/threads/ControlThread.hpp>
#include "core/threads/InputThread.hpp"

InputThread::InputThread(SystemThread * parent ) : SystemThread(std::string("InputThread"), THREAD_TYPES::THREAD_WORKER,parent)

{
  // create your own event sources etc here
  fprintf(stderr,"Created input thread \n");
  
  al_register_event_source(thread_event_queue_, al_get_keyboard_event_source());
  
  memset(key_, 0, sizeof(key_)); // do this when restarting as well
  
}

// destroy everything you have created bevore to prevent mem leaks
InputThread::~InputThread() {

}



void *InputThread::thread_(ALLEGRO_EVENT &event, void *args) {
  
  switch (event.type)
  {
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
      fprintf(stderr, "unexpected event in %s ! >%i<\n",name_.c_str(), event.type);
      break;
  }
  
  return NULL;
}

void InputThread::control_event_handler(size_t event) {
  if(event != RUNNING){
    stop();
    memset(key_, 0, sizeof(key_)); // do this when restarting as well
  }
}










// keyboard processing
void InputThread::keyboard_seen()
{
  for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
    key_[i] &= KEY_SEEN;
}

void InputThread::keyboard_update(ALLEGRO_EVENT *event)
{
  
  switch (event->type)
  {
    
    case ALLEGRO_EVENT_KEY_DOWN:
      fprintf(stderr, "Button pressed, input thread \n");
      key_[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
      break;
    case ALLEGRO_EVENT_KEY_UP:
      fprintf(stderr, "Button released, input thread \n");
      
      key_[event->keyboard.keycode] &= KEY_RELEASED;
      break;
  }
  process_keys();
}

void InputThread::process_keys()
{
  
  for (int processed_key_id = 0; processed_key_id < ALLEGRO_KEY_MAX; processed_key_id++)
  {
    
    if (key_[processed_key_id])
    {
      switch (processed_key_id)
      {
        case ALLEGRO_KEY_ESCAPE:
          // exit program
          send_control_event(EXIT);
          
          break;
        
        case ALLEGRO_KEY_R:
          // restart display thread
         send_control_event(RESTART);
          
          break;
        default:
          break;
      }
      fprintf(stderr, "Processed: %s \n", al_keycode_to_name(processed_key_id));
    }
    
    key_[processed_key_id] &= KEY_SEEN;
  }
}