#pragma once

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include <string>
#include <vector>
#include <memory>
#include <atomic>


#define LOG( tag, format, ... ) fprintf(stderr, ("["+std::string(tag)+"] "+ std::string(format)).c_str(), ##__VA_ARGS__)

//#define LOG(tag, ...) fprintf(stderr, "["  #tag "] " __VA_ARGS__)




enum SYSTEMTHREAD_EVENTS {
  THREAD_STATE_CHANGE_EVENT = ALLEGRO_GET_EVENT_TYPE('D', 'E', 'A', 'D'),
  THREAD_CONTROL_EVENT
};

enum THREAD_STATES {
  T_RUNNING,
  T_STOPPING, // pending
  T_STOPPED, // DONE
  T_DELETED
};

// worker threads search for the controller next up in their hirarchy
// controllers listen to the next controller in the hirarchy,if there is none then they run themselves, (this would only be the case with the first one)
enum class THREAD_TYPES {
  THREAD_NOTYPE,
  THREAD_WORKER,
  THREAD_CONTROLLER
};

class SystemThread {
private:
  ALLEGRO_THREAD *running_thread_ = NULL;
  static size_t obj_counter_;
  static ALLEGRO_MUTEX * object_counter_lock_;
  static std::vector<ALLEGRO_BITMAP*> bitmaps_;
protected:
  //core
  const std::string name_ = {};
  const char* TAG_ =NULL;
  const THREAD_TYPES type_;
  SystemThread *parent_ = NULL;
  std::vector<std::unique_ptr<SystemThread>> child_threads_={};
  
  //statistics
  size_t events_processed_ = 0;
  std::atomic<int> waiting_for_me_;
  
  // rendering structure
private:
  static ALLEGRO_DISPLAY* display_;
  static ALLEGRO_TIMER *fps_timer_;
protected:
  ALLEGRO_BITMAP* internal_bitmap_=NULL;
  ALLEGRO_MUTEX * internal_bitmap_lock_ = NULL;
  bool redraw_needed_ = false;
  size_t times_drawn_=0;
  bool is_fps_rendering_ = false;
  bool draw_to_display_=false;
  
  //thread structure
  ALLEGRO_EVENT_SOURCE *state_change_event_source_ = NULL;
  ALLEGRO_EVENT_SOURCE *control_event_source_ = NULL;
  ALLEGRO_EVENT_QUEUE *thread_event_queue_ = NULL; // also used by core function
  THREAD_STATES thread_state_ = THREAD_STATES::T_STOPPED;
  // also for user use
  void *thread_args_ = NULL;
  // for user use, needs to never be touched
  void *thread_retval_ = NULL;
  
  void update_thread_state(THREAD_STATES new_state);
  
  // just stuff that is needed all over the place
  static ALLEGRO_FONT* default_font_;
  
  
  
  
public:
  void enable_fps_rendering(bool to_display);
  
  SystemThread(const SystemThread &) = delete;
  void operator=(SystemThread const &) = delete;
  
  SystemThread(std::string name, THREAD_TYPES type, SystemThread *parent = NULL);
  
  virtual ~SystemThread();
  
  /* start can be overwritten, but you must call this start at the end
   * of your implementation in order to start the thread properly
   *  @params args will update the void * inside the thread_
   * */
  virtual bool start(void *args);
  
  /* stop can be overwritten, but you must call this start at the end
  * of your implementation in order to start the thread properly
  * */
  virtual bool stop();
  
  
  // maybe implement "restart"?
  
  virtual void *thread_(ALLEGRO_EVENT &event, void *args) = 0;
  void create_internal_bitmap();
  void adjust_bitmap_size();
  
  virtual void thread_event_filter_();
  void draw_internal_bitmap();
  virtual void draw()=0;
private:
  static void *thread_wrapper(ALLEGRO_THREAD *thr, void *arg); // make a wrapper so that the actual code only gets the event itself without handling the rest
  void thread_draw_wrapper();
public:
  
  bool wait_for_state(THREAD_STATES expected, bool blocking = true);
  
  bool send_control_event(const size_t control_event); // send it out
  
  virtual void control_event_handler(size_t event) = 0; // what happenes when received
  
  // need something like "send event to parent control via control"
  
  
  //getter
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
  ALLEGRO_BITMAP* get_internal_bitmap_(){
    assert(internal_bitmap_);
    return internal_bitmap_;
  }
  
  const char* get_name_(){
    return name_.c_str();
  }
  
  ALLEGRO_DISPLAY* get_display_(){
    return display_;
  }
  
  ALLEGRO_TIMER* get_fps_timer_(){
    return fps_timer_;
  }
  
  
  // util
public:
  static const char *repr(THREAD_STATES state);
  
  static const char *repr(THREAD_TYPES type);
  
  static void must_init(bool test, const char *description);
  
  static ALLEGRO_FONT* get_default_font();
  
  void print() const;
  
};
