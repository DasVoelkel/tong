#include "core/threads/ControlThread.hpp"
#include <core/threads/InputThread.hpp>
#include <core/threads/RenderThread.hpp>

ControlThread::ControlThread() : SystemThread(std::string("main"), THREAD_TYPES::THREAD_CONTROLLER),
                                 game_state(RUNNING)
                                 {
  
  timer_ = al_create_timer(1);
  al_start_timer(timer_);
  al_register_event_source(thread_event_queue_, al_get_timer_event_source(timer_));
  fprintf(stderr, "create ControlThreadObject\n");
  
  // start up here
  
  fprintf(stderr, "Controller Starting Program\n");

  child_threads_.emplace_back(std::make_unique<InputThread>());
  child_threads_.emplace_back(std::make_unique<RenderThread>());
  
                                   //render_thread::start(&control_event_source);
  //input_thread::start(&control_event_source);
  //update_program_state(THREAD_STATES::D_RUNNING);
  
  for(auto & child : child_threads_){
    child->start(NULL);
  }
}

ControlThread::~ControlThread() {

}


void ControlThread::control_event_handler(ALLEGRO_EVENT &event) {

fprintf(stderr,"ControlThread received control event, we should change our state to: %d type %d \n",event.user.data2, event.user.data1);


}
int i =0;
void *ControlThread::thread_(ALLEGRO_EVENT &event, void *args) {
  i++;
  if(i==4){
    ALLEGRO_EVENT event;
    event.user.data1 =GAME_STATE_CONTROL_CMD;
    event.user.data2 =RUNNING;
    send_control_event(event);
  }
  
  /*
  while (true)
  {
    switch (get_program_state())
    {
    case THREAD_STATES::D_RESTART:
      fprintf(stderr, "Restart Program\n");
      // close all threads, then restart them and recreate disp first !
      fprintf(stderr, "stop input \n");

      input_thread::stop();
      fprintf(stderr, "stop drawing \n");

      render_thread::stop();

      input_thread::start(&control_event_source);
      render_thread::start(&control_event_source);

      update_program_state(THREAD_STATES::D_RUNNING);

      break;
    case THREAD_STATES::D_RUNNING:
      al_wait_for_event(event_queue_control_thread, &event);

      break;

    case THREAD_STATES::D_EXIT:
      fprintf(stderr, "Exit Program\n");
      fprintf(stderr, "stop input \n");

      input_thread::stop();
      fprintf(stderr, "stop drawing \n");

      render_thread::stop();

      al_destroy_user_event_source(&control_event_source);
      al_destroy_font(get_font());
      audio_deinit();

      fprintf(stderr, "---EXIT--- \n");
      exit(0);

      break;

    default:
      // main button processing

      break;
    }
}*/
  
  return NULL;
}


