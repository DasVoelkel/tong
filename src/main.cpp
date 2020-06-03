#include <stdio.h>
#include <stdlib.h>


#include <core/threads/CompositorThread.hpp>

#define GAME_TITLE "Pong"





// --- program control ---


int main() // MAIN IS OUR CONTROL THREAD
{
  fprintf(stderr, "---START---\n");
  
 auto x = CompositorThread();
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
