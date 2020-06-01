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
  exit(0);
 
 ALLEGRO_EVENT_SOURCE tmps;
  al_init_user_event_source(&tmps);
 ALLEGRO_EVENT_QUEUE* tmp1 = al_create_event_queue();
  ALLEGRO_EVENT_QUEUE* tmp2 = al_create_event_queue();
  
  al_register_event_source(tmp1,&tmps);
  al_register_event_source(tmp2,&tmps);
  
  ALLEGRO_EVENT lel{};
  lel.type = 80000;
  al_emit_user_event(&tmps,&lel,NULL);
  
  ALLEGRO_EVENT ret;
  al_wait_for_event(tmp1,&ret);
  fprintf(stderr,"tmp1 %d \n",ret.type);
  al_wait_for_event(tmp2,&ret);
  fprintf(stderr,"tmp2 %d \n",ret.type);
  
  
}
