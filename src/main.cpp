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

#include <core/threads/input_thread.hpp>
#include <core/threads/render_thread.hpp>
#include <core/threads/ControlThread.hpp>

#define GAME_TITLE "Pong"

// --- program control ---
ALLEGRO_EVENT_SOURCE control_event_source;

const char *get_game_title() {
  return GAME_TITLE;
}

int main() // MAIN IS OUR CONTROL THREAD
{
  
  fprintf(stderr, "---START---\n");
  
  auto x = ControlThread();
  x.print();
  x.start(NULL);
  x.wait_for_state(STOPPED);
  ALLEGRO_EVENT
      event;
  /*
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
}*/
}
