#include "input_thread.hpp"
namespace input_thread
{



  //prototypes
  bool keyboard_input_thread_init(ALLEGRO_EVENT_SOURCE *control_event_source);
  void process_keys();
  void *input_thread(ALLEGRO_THREAD *thr, void *arg);

  void start(ALLEGRO_EVENT_SOURCE *event_source)
  {
    fprintf(stderr, "Input thread starting\n");

    memset(key, 0, sizeof(key));
    keyboard_input_thread_init(event_source);
  }

  void stop()
  {
    fprintf(stderr, "Input thread stopping\n");

    al_join_thread(get_p_input_thread(), NULL);
    al_destroy_event_queue(event_queue_input_thread);
  }

  


  ALLEGRO_THREAD *get_p_input_thread()
  {
    return p_input_thread;
  }

  bool keyboard_input_thread_init(ALLEGRO_EVENT_SOURCE *control_event_source)
  {
    event_queue_input_thread = al_create_event_queue();
    must_init(event_queue_input_thread, "draw-thread-queue");

    al_register_event_source(event_queue_input_thread, al_get_keyboard_event_source());
    al_register_event_source(event_queue_input_thread, control_event_source); // Gstate informant

    p_input_thread = al_create_thread(input_thread, NULL);
    if (p_input_thread)
    {
      al_start_thread(p_input_thread);
      return true;
    }
    else
    {
      p_input_thread = NULL;
      return false;
      ;
    }
  }

  void *input_thread(ALLEGRO_THREAD *thr, void *arg)
  {
    fprintf(stderr, "input thread started\n");
    ALLEGRO_EVENT event;

    if (get_program_state() == THREAD_STATES::D_RESTART || get_program_state() == THREAD_STATES::D_STARTING)
    {
      fprintf(stderr, "started input thread, waiting for go ahead \n");
      while (get_program_state() == THREAD_STATES::D_RESTART || get_program_state() == THREAD_STATES::D_STARTING)
      {
        al_wait_for_event(event_queue_input_thread, &event);
      }
      fprintf(stderr, "Go ahead granted: input thread \n");
    }

    while (get_program_state() != THREAD_STATES::D_EXIT && get_program_state() != THREAD_STATES::D_RESTART)
    {
      al_wait_for_event(event_queue_input_thread, &event);

  
    }
    fprintf(stderr, "input thread exit\n");
    return NULL;
  }

 
} // namespace input_thread