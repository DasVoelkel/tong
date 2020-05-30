#include "core/threads/ControlThread.hpp"

ControlThread::ControlThread() : SystemThread(std::string("main"), THREAD_TYPES::THREAD_CONTROLLER) {
  
  timer_ = al_create_timer(1);
  al_start_timer(timer_);
  al_register_event_source(thread_event_queue_, al_get_timer_event_source(timer_));
  fprintf(stderr, "create ControlThreadObject\n");
  
}

ControlThread::~ControlThread() {

}


int i = 0;

void *ControlThread::thread_(ALLEGRO_EVENT &event, void *args) {
  static int i = 0;
  fprintf(stderr, "thread_ controlthread %d \n", i);
  i++;
  if (i > 5)
    stop_self();
  
  return NULL;
}
