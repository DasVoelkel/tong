#include <core/threads/RenderThread.hpp>

#include <allegro5/allegro_color.h>
#include <core/threads/CompositorThread.hpp>


RenderThread::RenderThread(SystemThread * parent ) : SystemThread(std::string("RenderThread"), THREAD_TYPES::THREAD_WORKER,parent)

{
  
  // testing

  
  
}

// destroy everything you have created before to prevent mem leaks
RenderThread::~RenderThread() {

}

void RenderThread::startup() {
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_register_event_source(event_queue, al_get_mouse_event_source());
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
      LOG(TAG_,"key : %d \n",event.keyboard.keycode);
    
  }
  
  return NULL;
}
void RenderThread::control_event_handler(UserEvent &event) {

}

void RenderThread::draw() {
auto color = al_color_name("black");
  al_draw_textf(get_default_font(), color , 1, 1, ALLEGRO_ALIGN_LEFT, "Drawn threads: %d ",1);
  
}

void RenderThread::shutdown() {

}

