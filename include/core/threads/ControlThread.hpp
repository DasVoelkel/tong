#include "core/threads/class/SystemThreads.hpp"
#include <string>

class ControlThread : public SystemThread {

private:
  ALLEGRO_TIMER *timer_;

public:
  ControlThread();
  
  ~ControlThread();
  
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  
};