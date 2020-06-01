#include <core/threads/RenderThread.hpp>

#include <allegro5/allegro_color.h>
RenderThread::RenderThread() : SystemThread(std::string("RenderThread"), THREAD_TYPES::THREAD_WORKER)

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

}



void *RenderThread::thread_(ALLEGRO_EVENT &event, void *args) {
  
  al_set_target_bitmap(buffer_);
  al_clear_to_color(al_color_name("black"));
  
  al_draw_text(get_default_font(), al_color_name("white"), 1, 1, ALLEGRO_ALIGN_LEFT, "Hello world!");
  
  al_set_target_backbuffer(disp_);
  al_draw_scaled_bitmap(buffer_, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);
  al_flip_display();
  
  return NULL;
}
void RenderThread::control_event_handler(ALLEGRO_EVENT &event) {
  
}