#include "core/threads/class/SystemThreads.hpp"
#include <string>
#include <atomic>
#include <map>


const size_t GAME_STATE_CONTROL_CMD = 1;



enum  CONTROL_CMD{
RUNNING = 0,
RESTART,
EXIT,
};


class DisplayThread : public SystemThread {

private:
  void startup()override;
  void shutdown()override;


public:

  int counter=0;
  
public:
  DisplayThread();
  
   ~DisplayThread() override;
  
   void *thread_(ALLEGRO_EVENT &event, void *args) override;
   void control_event_handler(UserEvent &event) override;
  void draw() override;
};