#include <core/threads/RenderThread.hpp>

#include <allegro5/allegro_color.h>
#include <core/threads/CompositorThread.hpp>


RenderThread::RenderThread(SystemThread * parent ) : SystemThread(std::string("RenderThread"), THREAD_TYPES::THREAD_WORKER,parent)

{
  // create your own event sources etc here
  // start up here
  
  
  
  // testing
  al_register_event_source(thread_event_queue_, al_get_keyboard_event_source());
  al_register_event_source(thread_event_queue_, al_get_mouse_event_source());
  
  //AGUI
  
  
}

// destroy everything you have created before to prevent mem leaks
RenderThread::~RenderThread() {
  

  
  
}


bool redraw = true;
void *RenderThread::thread_(ALLEGRO_EVENT &event, void *args) {
  
  switch (event.type) {
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
    
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
      break;
    case ALLEGRO_EVENT_KEY_DOWN:
      LOG(TAG_,"ley : %d \n",event.keyboard.keycode);
      if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
        send_control_event(EXIT);
        break;
      }
  }
  
  return NULL;
}
void RenderThread::control_event_handler(size_t event) {
 if(event != RUNNING){
 stop();
 }
}

void RenderThread::draw() {
  LOG(TAG_," %p %p %p\n",internal_bitmap_,get_default_font());
auto color = al_color_name("black");

  al_draw_textf(get_default_font(), color , 1, 1, ALLEGRO_ALIGN_LEFT, "Drawn threads: %d ",1);
  
}
