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


class CompositorThread : public SystemThread {


public:
   std::atomic<CONTROL_CMD> game_state;
public:
  CompositorThread();
  
   ~CompositorThread() override;
  
   void *thread_(ALLEGRO_EVENT &event, void *args) override;
   void control_event_handler(size_t event) override;
  void draw() override;
};