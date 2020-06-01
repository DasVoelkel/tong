#include <core/threads/class/SystemThreads.hpp>


#define KEY_SEEN 1
#define KEY_RELEASED 2

class InputThread : public SystemThread {

private:
// stuff only this class should know
  unsigned char key_[ALLEGRO_KEY_MAX];
  
  void keyboard_seen();
  void keyboard_update(ALLEGRO_EVENT *event);
  void process_keys();
  
public:
  // stuff everyone can access
public:
  InputThread(SystemThread * parent = NULL);
  
  ~InputThread() override;
  
  virtual void control_event_handler(size_t event) override;
  // only absolutely neccessary override
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  
};