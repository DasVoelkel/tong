#include "core/threads/DisplayThread.hpp"
#include <core/threads/GuiThread.hpp>
//#include <core/threads/RenderThread.hpp>
#include <jansson.h>
#include <allegro5/allegro_color.h>


DisplayThread::DisplayThread() : SystemThread(std::string("Compositor"), THREAD_TYPES::THREAD_CONTROLLER) {

}

DisplayThread::~DisplayThread() {
}

void DisplayThread::startup() {
   counter =0;
  
  al_show_mouse_cursor(get_display_());
  
  al_set_window_title(get_display_(), "Agui - Example");
  
  child_threads_.emplace_back(std::make_unique<GuiThread>(this));
  
  for (auto &child : child_threads_) {
    //child->enable_fps_rendering();
  }
  
  for (auto &child : child_threads_) {
    child->start(NULL);
  }
  
  
}

void DisplayThread::shutdown() {
  for (auto &child : child_threads_) {
    child->stop();
  }
  
  for (auto &child : child_threads_) {
    child->wait_for_state(THREAD_STATES::T_STOPPED);
  }
  child_threads_.clear();
  
  
}

void DisplayThread::control_event_handler(UserEvent &event) {
  
  LOG(TAG_,"Custom event %d \n",event.event);
  
}

void *DisplayThread::thread_(ALLEGRO_EVENT &event, void *args) {
  switch (event.type) {
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      //adjust_bitmap_size();
      break;
    default:
      break;
    
  }
  return NULL;
}


void DisplayThread::draw() {
  counter++;
  //al_clear_to_color(al_color_name("white"));
  
}



