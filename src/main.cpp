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

#include "../add_later/input_thread.hpp"
#include "../add_later/render_thread.hpp"
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
  x.wait_for_state(T_STOPPED);
  ALLEGRO_EVENT
      event;
  
}
