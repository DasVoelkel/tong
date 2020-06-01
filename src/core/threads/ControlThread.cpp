#include "core/threads/ControlThread.hpp"
#include <core/threads/InputThread.hpp>
#include <core/threads/RenderThread.hpp>

ControlThread::ControlThread() : SystemThread(std::string("main"), THREAD_TYPES::THREAD_CONTROLLER),
                                 game_state(RUNNING)
                                 {
  

  fprintf(stderr, "create ControlThreadObject\n");
  
  // start up here
  
  fprintf(stderr, "Controller Starting Program\n");

  child_threads_.emplace_back(std::make_unique<InputThread>(this));
  child_threads_.emplace_back(std::make_unique<RenderThread>(this));
  
                                   //render_thread::start(&control_event_source);
  //input_thread::start(&control_event_source);
  //update_program_state(THREAD_STATES::D_RUNNING);
  
  for(auto & child : child_threads_){
    child->start(NULL);
  }
}

ControlThread::~ControlThread() {

}


void ControlThread::control_event_handler(size_t event) {

fprintf(stderr,"ControlThread received control event, we should change our state to:  %d \n",event);
  
  if(game_state == event){
    fprintf(stderr,"Already in state:  %d \n",event);
  
    return ;
  }

game_state =(GAME_STATE) event;
  switch (event) {
    case RUNNING:
      break;
    case RESTART:
    
      for(auto & child : child_threads_){
        child->wait_for_state(THREAD_STATES::T_STOPPED);
      }
      
      for(auto & child : child_threads_){
        child->start(NULL);
      }
      game_state = RUNNING;
      break;
    case EXIT:

      for(auto & child : child_threads_){
        child->wait_for_state(THREAD_STATES::T_STOPPED);
      }
      stop();
      break;
    default:
      assert(false);
      break;
  
  }

}


void *ControlThread::thread_(ALLEGRO_EVENT &event, void *args) {

  
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


