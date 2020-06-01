#include "core/threads/class/SystemThreads.hpp"
#include <string>
#include <atomic>


const size_t GAME_STATE_CONTROL_CMD = 1;
enum GAME_STATE{
RUNNING = 0,
RESTART,
EXIT
};


class ControlThread : public SystemThread {

private:
  ALLEGRO_TIMER *timer_;
public:
   std::atomic<GAME_STATE> game_state;
public:
  ControlThread();
  
   ~ControlThread() override;
  
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  virtual void control_event_handler(ALLEGRO_EVENT & event) override;
};