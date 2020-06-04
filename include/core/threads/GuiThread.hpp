#include <core/threads/class/SystemThreads.hpp>
#include <rendering/gui/WidgetCreator.hpp>


class GuiThread : public SystemThread {

private:
// stuff only this class should know
  
  WidgetCreator* creator;
//Globals
  ALLEGRO_DISPLAY *display = NULL;
  ALLEGRO_TIMER *timer = NULL;
  ALLEGRO_EVENT event;
  ALLEGRO_EVENT_QUEUE *queue = NULL;
  bool done = false;
  agui::Gui *gui = NULL;
  agui::Allegro5Input* inputHandler = NULL;
  agui::Allegro5Graphics* graphicsHandler = NULL;
  
  agui::Font *defaultFont = NULL;
  
  void startup() override;
  void shutdown() override;
  void draw() override;
public:
  // stuff everyone can access
  
public:
  GuiThread(SystemThread * parent = NULL);
  
  ~GuiThread() override;
  
  void control_event_handler(UserEvent &event) override;
  
  // only absolutely neccessary override
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) override;
  
};