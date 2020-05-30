#include "core/GameController.hpp"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>

GameController::GameController()
{
  // init all libs
  // init essentials
  must_init(al_init(), "allegro");
  must_init(al_install_mouse(), "al_install_mouse");
  must_init(al_install_keyboard(), "keyboard");

  must_init(al_init_image_addon(), "al_init_image_addon");
  must_init(al_init_primitives_addon(), "al_init_primitives_addon");
  must_init(al_init_ttf_addon(), "al_init_ttf_addon");

  // init event notifier
  al_init_user_event_source(&control_event_source);
  event_queue_control_thread = al_create_event_queue();
  must_init(event_queue_control_thread, "event-queue-control-thread");
  al_register_event_source(event_queue_control_thread, &control_event_source);

  // create all game system threads here

  // start control thread

  //end
  update_state(GAMESTATE::GAME_INITED);
}

void GameController::update_state(GAMESTATE new_program_state)
{
  ALLEGRO_EVENT g_state_event;
  g_state_event.type = CONTROLLER_EVENT_NUM;
  state_ = new_program_state;
  if (al_emit_user_event(&control_event_source, &g_state_event, NULL))
  {
    al_broadcast_cond(state_cond_); // condition anything
    fprintf(stderr, "success sending event change! changing to %s\n ", repr(state_));
  }
  else
  {
    fprintf(stderr, "fail sending event change! \n");
    assert(false);
  }
}

void *GameController::thread_(ALLEGRO_THREAD *thr, void *arg)
{
  ALLEGRO_EVENT event;
  while (get_state() != GAMESTATE::GAME_EXITING)
  {
    al_wait_for_event(event_queue_control_thread, &event);
  }

  switch (event.type)
  {
  case CONTROLLER_EVENT_NUM:
    // check if we need to close the thread

    fprintf(stderr, "Controller got %i \n", get_program_state());

    break;

  default:
    fprintf(stderr, "unexpected event in input thread! >%i<\n", event.type);
    break;
  }
}

void GameController::wait_until_done()
{
  al_lock_mutex(state_mutex_);
  while (state_ != GAMESTATE::GAME_EXITED)
    al_broadcast_cond(state_cond_);
  al_unlock_mutex(state_mutex_);
}

//util
void GameController::must_init(bool test, const char *description)
{
  if (test)
    return;

  fprintf(stderr, "couldn't initialize %s\n", description);
  exit(1);
}

const char *GameController::repr(GAMESTATE &state)
{

  switch (state)
  {
  case GAMESTATE::GAME_UNINITIALIZED:
    return "   Game State --GAME_UNINITIALIZED-- ";
    break;
  case GAMESTATE::GAME_INITED:
    return "   Game State --GAME_INITED-- ";
    break;
  case GAMESTATE::GAME_STARTING:
    return "   Game State --GAME_STARTING-- ";
    break;
  case GAMESTATE::GAME_EXITED:
    return "   Game State --GAME_EXITED-- ";
    break;

  case GAMESTATE::GAME_EXITING:
    return "   Game State --GAME_EXITING-- ";
    break;
  default:
    return "   Game State --UNKNOWN-- ";
    break;
  }
}

GAMESTATE GameController::get_state()
{
  return state_;
}
