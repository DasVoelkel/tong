#pragma once
#include <string>
#include <vector>
#include <memory>
#include <core/threads/SystemThreads.hpp>

const int CONTROLLER_EVENT_NUM = 200;

enum class GAMESTATE
{
  GAME_UNINITIALIZED,
  GAME_INITED,
  GAME_STARTING,
  GAME_EXITING,
  GAME_EXITED

};

class GameController
{
private:
  //core
  std::string name_ = {};
  GAMESTATE state_ = GAMESTATE::GAME_UNINITIALIZED;
  ALLEGRO_EVENT_SOURCE control_event_source;
  ALLEGRO_EVENT_QUEUE *event_queue_control_thread;

  std::vector<std::unique_ptr<SystemThread>> threads_ = {};

  void *thread_(ALLEGRO_THREAD *thr, void *arg);

public:
  GameController(const GameController &) = delete;
  void operator=(GameController const &) = delete;

  GameController();

  void update_state(GAMESTATE new_program_state);
  void run();
  void wait_until_done();
  GAMESTATE get_state();
  //util
public:
  static void must_init(bool test, const char *description);
  static const char *repr(GAMESTATE &state);
};
