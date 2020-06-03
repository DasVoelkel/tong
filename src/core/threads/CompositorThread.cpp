#include "core/threads/CompositorThread.hpp"
#include <core/threads/InputThread.hpp>
#include <core/threads/RenderThread.hpp>
#include <jansson.h>

CompositorThread::CompositorThread() : SystemThread(std::string("Compositor"), THREAD_TYPES::THREAD_CONTROLLER), game_state(RUNNING) {
  

  al_show_mouse_cursor(get_display_());
  al_set_window_title(get_display_(),"Agui - Example");
  
  enable_fps_rendering(true);
  child_threads_.emplace_back(std::make_unique<RenderThread>(this));
  
  for (auto &child : child_threads_) {
    child->create_internal_bitmap();
  }
  
  for (auto &child : child_threads_) {
    //child->start(NULL);
  }
  
}

CompositorThread::~CompositorThread() {
}


void CompositorThread::control_event_handler(size_t event) {
  
  

  game_state = (CONTROL_CMD) event;
  switch (event) {
    case RUNNING:
      break;
    case RESTART:
      for (auto &child : child_threads_) {
        child->wait_for_state(THREAD_STATES::T_STOPPED);
      }
      
      for (auto &child : child_threads_) {
        child->start(NULL);
      }
      game_state = RUNNING;
      break;
    case EXIT:
      
      for (auto &child : child_threads_) {
        child->wait_for_state(THREAD_STATES::T_STOPPED);
      }
      stop();
      break;

    default:
      assert(false);
      break;
    
  }
  
}

#include <allegro5/allegro_color.h>


void *CompositorThread::thread_(ALLEGRO_EVENT &event, void *args) {

  
  
  switch (event.type)
  {
    case ALLEGRO_EVENT_TIMER:
    {
      LOG(TAG_,"FILTER TIMER CORRECTLY! \n ");
    }
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      // closing the window
     LOG(TAG_,"ALLEGRO_EVENT_DISPLAY_CLOSE\n");
      send_control_event(EXIT);
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
  
      LOG(TAG_,"ALLEGRO_EVENT_DISPLAY_RESIZE , GUI resize\n");

      
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      fprintf(stderr, "switched into display\n");
      //gui->resizeToDisplay();
      
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
      fprintf(stderr, "switched out of display\n");
      break;
  
    default:
      //fprintf(stderr, "unexpected event in %s ! >%i<\n",name_.c_str(), event.type);
      break;
    
  }
  
  return NULL;
}

void CompositorThread::draw() {
  
  al_clear_to_color(al_color_name("white"));
  al_draw_textf(get_default_font(), al_color_name("black"), 1, 1, ALLEGRO_ALIGN_LEFT, "Drawn threads: %d ",times_drawn_);
  
}


