#include "core/threads/CompositorThread.hpp"
#include <core/threads/InputThread.hpp>
//#include <core/threads/RenderThread.hpp>
#include <jansson.h>

#include <allegro5/allegro_color.h>


CompositorThread::CompositorThread() : SystemThread(std::string("Compositor"), THREAD_TYPES::THREAD_CONTROLLER) {

}

CompositorThread::~CompositorThread() {
}

void CompositorThread::startup() {
   counter =0;
  
  al_show_mouse_cursor(get_display_());
  
  al_set_window_title(get_display_(), "Agui - Example");
  
  enable_fps_rendering();
  
  child_threads_.emplace_back(std::make_unique<InputThread>(this));
  
  for (auto &child : child_threads_) {
    //child->enable_fps_rendering();
  }
  
  for (auto &child : child_threads_) {
    child->start(NULL);
  }
  
  LOG(TAG_,"Children after start : %d \n",child_threads_.size());
  
}

void CompositorThread::shutdown() {
  for (auto &child : child_threads_) {
    child->stop();
  }
  
  for (auto &child : child_threads_) {
    child->wait_for_state(THREAD_STATES::T_STOPPED);
  }
  child_threads_.clear();
  
  LOG(TAG_,"Children left: %d \n",child_threads_.size());
  
}








void CompositorThread::control_event_handler(UserEvent &event) {
  
  LOG(TAG_,"Custom event %d \n",event.event);
  

  
}


void *CompositorThread::thread_(ALLEGRO_EVENT &event, void *args) {
  switch (event.type) {
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      //adjust_bitmap_size();
      break;
    default:
      break;
    
  }
  return NULL;
}


void CompositorThread::draw() {
  counter++;
  al_clear_to_color(al_color_name("white"));
  al_draw_textf(get_default_font(), al_color_name("black"), 1, 1, ALLEGRO_ALIGN_LEFT, "Drawn threads: %d ", counter);
  
}



