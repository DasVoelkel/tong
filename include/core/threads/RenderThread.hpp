#include <core/threads/class/SystemThreads.hpp>

#include <rendering/gui/WidgetCreator.hpp>

class RenderThread : public SystemThread {

private:
// stuff only this class should know

  /* GUI */
  

public:
  // stuff everyone can access
public:
  RenderThread(SystemThread * parent = NULL);
  RenderThread() = delete;
  
  virtual ~RenderThread();
  
  // only absolutely neccessary override
   void *thread_(ALLEGRO_EVENT &event, void *args) override;
   void control_event_handler(size_t event) override;
  void draw() override ;
  
};