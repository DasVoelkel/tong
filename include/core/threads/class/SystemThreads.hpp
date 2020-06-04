#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <map>

#define LOG(tag, format, ...) fprintf(stderr, ("["+std::string(tag)+"] "+ std::string(format)).c_str(), ##__VA_ARGS__)

//#define LOG(tag, ...) fprintf(stderr, "["  #tag "] " __VA_ARGS__)











/* SystemThread Events meant for internal control of the state of an object.
 *
 * */
enum class SYSTEMTHREAD_EVENTS : size_t {
  THREAD_EVENT_GLOBAL = ALLEGRO_GET_EVENT_TYPE('D', 'E', 'A', 'D'),     // Signal all SystemThread objects, only used for shutdown/restart right now
  THREAD_EVENT_CHAIN,                                                               // Signal this and every Object below this one
  THREAD_EVENT_LOCAL,                                                               // signal the current systemthread object
  THREAD_EVENT_CUSTOM,                                                              // will always be passed through to the handler
  THREAD_EVENT_STATE_CHANGE                                                         // Only used by wait_for_thread, otherwise this will not be touched
};


// Thread States , a thread will start in "stopped" state
enum class THREAD_STATES : size_t {
  T_RUNNING,    // Thread internally is running
  T_STOPPED,    // Thread either stopped or never started
  T_DELETED     // destructor has been called, object invalid
};

// what commands can get through the global handler?
enum class GLOBAL_THREAD_CMD : size_t {
  G_EXIT,
  G_RESTART
};

// chain events
enum class CHAIN_THREAD_CMD : size_t {
  C_EXIT
};

// local commands
enum class LOCAL_THREAD_CMD : size_t {
  L_STOPPING,
  L_STARTING,
  L_DELETING
};


// worker threads search for the controller next up in their hirarchy
// controllers listen to the next controller in the hirarchy,if there is none then they run themselves, (this would only be the case with the first one)
enum class THREAD_TYPES : size_t {
  THREAD_CONTROLLER,        //
  THREAD_WORKER
  
};


struct UserEvent {
  SYSTEMTHREAD_EVENTS event_type;
  size_t event;
};

static void UserEventDtor(ALLEGRO_USER_EVENT *event) {
  free((void *) event->data1);
}


class SystemThread {
  //static to all
private:
  ALLEGRO_THREAD *running_thread_ = NULL;
  
  // What objects have rendering enabled?
  static std::vector<std::pair<size_t,SystemThread*>> render_list_;
  static ALLEGRO_MUTEX *render_list_lock_;
  
  
  static size_t obj_counter_;
  static ALLEGRO_MUTEX *objects_lock_;
  
  // FPS Timer, will trigger 'draw' function when triggered
  static ALLEGRO_TIMER *fps_timer_;
  
  // helper to something that is globally unique
  static ALLEGRO_FONT *default_font_;
  
  // global source, will effect any object
  static ALLEGRO_EVENT_SOURCE *global_source_;
  
  // Used by controllers to control their chain
  ALLEGRO_EVENT_SOURCE *chain_source_=NULL;
  
  // Used by anyone for local state changes
  ALLEGRO_EVENT_SOURCE *local_source=NULL;
  
  // Used for wait_for_state change
  ALLEGRO_EVENT_SOURCE * state_change_source=NULL;
  
  // There is at most only 1 display per program, if you need more you gotta handle that yourself.
  static ALLEGRO_DISPLAY *display_;
  
  
protected:
  //core
  // Thread name
  const std::string name_ = {};
  // [name] for a macro
  const char *TAG_ = NULL;
  const THREAD_TYPES type_;
  
  SystemThread *parent_ = NULL;
  
  // how many threads are waiting for me right now ?
  std::atomic<int> waiting_for_me_;
  
  // all children should be added here so even with new the deleter will be called
  std::vector<std::unique_ptr<SystemThread>> child_threads_;

private:
  // Only called by root
  void create_root_display();
  
  // called before thread_ loops indefinitely
  virtual void startup() = 0;
  
  // called before the thread changes to "stopped" state
  virtual void shutdown() = 0;
  
  // will be called when there is any event to handle unless it's internal
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) = 0;
  
  // If there is a custom event received it will reach through to this spot
  virtual void control_event_handler(UserEvent &event) = 0;
  
  // Default implementation will filter out global and local events, you can overwrite it if you want
  virtual void thread_event_filter_();
  
  
  static void *thread_wrapper(ALLEGRO_THREAD *thr, void *arg); // make a wrapper so that the actual code only gets the event itself without handling the rest
  
  // called from internally
  // redraws will be triggered whenever 'redraw_needed' is true and the thread has no waiting events,
  // the pre and post operations will be done implicitly,
  // use this if you don't want to mess around with Bitmap Targets
  virtual void draw();
  
  void change_state(THREAD_STATES new_state);
  
  void draw_to_display();
protected:
  
  // control
  bool redraw_needed_ = false;
  
  //thread structure
  ALLEGRO_EVENT_SOURCE *local_event_source_ = NULL;
  ALLEGRO_EVENT_SOURCE *chain_event_source_ = NULL;
  
  // Everything lands in here, as we don't want to "stall" when something happenes we don't expect
  ALLEGRO_EVENT_QUEUE *event_queue = NULL;
  
  // Starting state should always be stopped
  THREAD_STATES thread_state_ = THREAD_STATES::T_STOPPED;
  
  // Args will pass through "start" to the thread itself
  void *thread_args_ = NULL;
  
  // will be made accessible
  void *thread_retval_ = NULL;

public:
  virtual ~SystemThread();
  
  SystemThread(const SystemThread &) = delete;
  
  void operator=(SystemThread const &) = delete;
  
  // Controller Threads will additionally have a chain source to send to
  SystemThread(std::string name, THREAD_TYPES type, SystemThread *parent = NULL);
  
  
  // dispatch any type of command
  template<class T>
  void dispatch_event(SYSTEMTHREAD_EVENTS type, T event);
  
  
  /* start can be overwritten, but you must call this start at the end
   * of your implementation in order to start the thread properly
   *  @params args will update the void * inside the thread_
   * */
  virtual bool start(void *args);
  
  /* stop can be overwritten, but you must call this start at the end
  * of your implementation in order to start the thread properly
  * */
  virtual bool stop();
  
  // block / or not , until the desired state is reached
  bool wait_for_state(THREAD_STATES expected, bool blocking = true);
  
  // memory will stay allocated until the end of life of the object
  void disable_fps_rendering();
  
  // enable internal redrawing
  void enable_fps_rendering(size_t order);
  
  
  void enable_display_events();
  
  void disable_display_events();
  
  void close() {
    dispatch_event(SYSTEMTHREAD_EVENTS::THREAD_EVENT_GLOBAL, GLOBAL_THREAD_CMD::G_EXIT);
  }
  
  void restart() {
    dispatch_event(SYSTEMTHREAD_EVENTS::THREAD_EVENT_GLOBAL, GLOBAL_THREAD_CMD::G_RESTART);
    
  }

public:
  
  THREAD_STATES &get_state_() {
    return thread_state_;
  }
  
  void *get_retval_() {
    return thread_retval_;
  }
  
  THREAD_TYPES get_type_() {
    return type_;
  }
  
  const char *get_name_() {
    return name_.c_str();
  }
  
  ALLEGRO_DISPLAY *get_display_() {
    return SystemThread::display_;
  }
  
  ALLEGRO_TIMER *get_fps_timer_() {
    return fps_timer_;
  }
  
  bool is_fps_rendering_enabled();
  
  bool is_display_events_enabled();
  
  // util
public:
  static const char *repr(THREAD_STATES state);
  
  static const char *repr(THREAD_TYPES type);
  
  static const char *repr(SYSTEMTHREAD_EVENTS event);
  
  static const char *repr(GLOBAL_THREAD_CMD event);
  
  static const char *repr(CHAIN_THREAD_CMD event);
  
  static const char *repr(LOCAL_THREAD_CMD event);
  
  
  static void must_init(bool test, const char *description);
  
  static ALLEGRO_FONT *get_default_font();
  
  void print() const;
  
  
};
