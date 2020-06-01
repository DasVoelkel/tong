#include <core/threads/class/SystemThreads.hpp>

class RenderThread : public SystemThread {

private:
// stuff only this class should know
ALLEGRO_DISPLAY * disp_;
const int  BUFFER_W =  480;
const int  BUFFER_H =  240;
const int  DISP_SCALE =  3;
const int  DISP_W =  (BUFFER_W * DISP_SCALE);
const int  DISP_H =  (BUFFER_H * DISP_SCALE);
  
  ALLEGRO_BITMAP *buffer_ = NULL;
  
  ALLEGRO_TIMER *fps_timer_ = NULL;

  
public:
  // stuff everyone can access
public:
  RenderThread();
  
  virtual ~RenderThread();
  
  // only absolutely neccessary override
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  virtual void control_event_handler(ALLEGRO_EVENT & event) override;
  
  
};