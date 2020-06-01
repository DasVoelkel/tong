#include <core/threads/class/SystemThreads.hpp>

class InputThread : public SystemThread {

private:
// stuff only this class should know
public:
  // stuff everyone can access
public:
  InputThread();
  
  ~InputThread() override;
  
  virtual void control_event_handler(ALLEGRO_EVENT & event) override;
  // only absolutely neccessary override
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  
};