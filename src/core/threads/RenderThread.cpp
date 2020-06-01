#include <core/threads/RenderThread.hpp>

#include <allegro5/allegro_color.h>
#include <core/threads/ControlThread.hpp>

RenderThread::RenderThread(SystemThread * parent ) : SystemThread(std::string("RenderThread"), THREAD_TYPES::THREAD_WORKER,parent)

{
  // create your own event sources etc here
  
  
  // start up here
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  
  disp_ = al_create_display(DISP_W, DISP_H);
  must_init(disp_, "display");
  buffer_ = al_create_bitmap(BUFFER_W, BUFFER_H);
  
  fps_timer_ = al_create_timer(1.0 / 60.0);
  must_init(fps_timer_, "draw-thread-timer");
  
  al_register_event_source(thread_event_queue_, al_get_timer_event_source(fps_timer_));
  al_register_event_source(thread_event_queue_, al_get_display_event_source(disp_)); // Gstate informant
  
  al_start_timer(fps_timer_);
  
}

// destroy everything you have created bevore to prevent mem leaks
RenderThread::~RenderThread() {
  al_destroy_bitmap(buffer_);
  al_destroy_display(disp_);
  al_destroy_timer(fps_timer_);
}



void *RenderThread::thread_(ALLEGRO_EVENT &event, void *args) {
  switch (event.type)
  {
    case ALLEGRO_EVENT_TIMER:
    {
  
      drawn_threads_++;
      al_set_target_bitmap(buffer_);
      al_clear_to_color(al_color_name("black"));
  
      al_draw_textf(get_default_font(), al_color_name("white"), 1, 1, ALLEGRO_ALIGN_LEFT, "Drawn threads: %d ",drawn_threads_);
  
      al_set_target_backbuffer(disp_);
      al_draw_scaled_bitmap(buffer_, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
      al_flip_display();
      
      break;
    }
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
      // closing the window
      fprintf(stderr, "EVENT CLOSE DISPLAY >%i<\n", event.type);
    
      send_control_event(EXIT);
      
      break;
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      
      al_acknowledge_resize(event.display.source);
      
      //Resize Agui
      //menu_gui->resizeToDisplay();
      
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
      fprintf(stderr, "switched into display\n");
    
      break;
    case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
      fprintf(stderr, "switched out of display\n");
      break;
    default:
      fprintf(stderr, "unexpected event in %s ! >%i<\n",name_.c_str(), event.type);
      break;
  }
  
  

  
  return NULL;
}
void RenderThread::control_event_handler(size_t event) {
  fprintf(stderr, "%s HANDLING CONTROL EVENT %d \n",name_.c_str(),event);

 if(event != RUNNING){
 stop();
   drawn_threads_ =0 ;
 }
}