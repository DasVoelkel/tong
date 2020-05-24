#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

#include <core/types.hpp>
#include <core/helper.hpp>
#include <audio/audio.hpp>

#include <core/threads/input_thread.hpp>
#include <core/threads/render_thread.hpp>

// --- program control ---
std::atomic<THREAD_STATES> program_state{THREAD_STATES::D_STARTING};
ALLEGRO_EVENT_SOURCE control_event_source;

void update_program_state(THREAD_STATES new_program_state)
{
  ALLEGRO_EVENT g_state_event;
  g_state_event.type = G_STATE_CHANGE_EVENT_NUM;
  program_state = new_program_state;
  if (al_emit_user_event(&control_event_source, &g_state_event, NULL))
  {
    fprintf(stderr, "success sending event change! changing to %s\n ", repr(new_program_state));
  }
  else
  {
    fprintf(stderr, "fail sending event change! \n");
    assert(false);
  }
}

THREAD_STATES get_program_state()
{
  return program_state;
}

int main() // MAIN IS OUR CONTROL THREAD
{

  // init essentials
  must_init(al_init(), "allegro");
  must_init(al_install_mouse(), "al_install_mouse");
  must_init(al_install_keyboard(), "keyboard");

  must_init(al_init_image_addon(), "al_init_image_addon");
  must_init(al_init_primitives_addon(), "al_init_primitives_addon");
  must_init(al_init_ttf_addon(), "al_init_ttf_addon");

  fprintf(stderr, "---START---\n");

  //audio_init(); // TODO change this so audio can also be reinited, may not be neccessary

  al_init_user_event_source(&control_event_source);

  ALLEGRO_EVENT_QUEUE *event_queue_control_thread = al_create_event_queue();
  must_init(event_queue_control_thread, "event-queue-control-thread");
  al_register_event_source(event_queue_control_thread, &control_event_source);

  ALLEGRO_EVENT event;

  while (true)
  {
    switch (get_program_state())
    {
    case THREAD_STATES::D_STARTING:
      fprintf(stderr, "Starting Program\n");
      render_thread::start(&control_event_source);
      input_thread::start(&control_event_source);
      update_program_state(THREAD_STATES::D_RUNNING);

      break;

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
  }
}
