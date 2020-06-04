#include <core/threads/GuiThread.hpp>
#include <rendering/gui/WidgetCreator.hpp>
#include <allegro5/allegro_color.h>

GuiThread::GuiThread(SystemThread *parent) : SystemThread(std::string("GuiThread"), THREAD_TYPES::THREAD_WORKER, parent) {

  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_show_mouse_cursor(get_display_());
  al_set_window_title(get_display_(),"Mug PRgoGraaam");
  al_set_new_bitmap_flags(ALLEGRO_AUTO_CONVERT_BITMAPS);
  
  // Agui
  
  //Set the image loader
  agui::Image::setImageLoader(new agui::Allegro5ImageLoader);
  
  //Set the font loader
  agui::Font::setFontLoader(new agui::Allegro5FontLoader);
  
  //Instance the input handler
  inputHandler = new agui::Allegro5Input();
  
  //Instance the graphics handler
  graphicsHandler = new agui::Allegro5Graphics();
  
  //Allegro does not automatically premultiply alpha so let Agui do it
  agui::Color::setPremultiplyAlpha(true);
  
  //Instance the gui
  gui = new agui::Gui();
  
  //Set the input handler
  gui->setInput(inputHandler);
  
  //Set the graphics handler
  gui->setGraphics(graphicsHandler);
  
  defaultFont = agui::Font::load("assets/DejaVuSans.ttf",16);
  
  //Setting a global font is required and failure to do so will crash.
  agui::Widget::setGlobalFont(defaultFont);
  
  creator = new WidgetCreator(gui);
}

GuiThread::~GuiThread() {
  gui->getTop()->clear();
  delete creator;
  creator = NULL;
  delete gui;
  gui = NULL;
  delete inputHandler;
  delete graphicsHandler;
  inputHandler = NULL;
  graphicsHandler = NULL;
  
  delete defaultFont;
  defaultFont = NULL;
}






void GuiThread::startup() {
  enable_fps_rendering(0);
  enable_display_events();
  
}

void GuiThread::shutdown() {
disable_fps_rendering();


}


void *GuiThread::thread_(ALLEGRO_EVENT &event, void *args) {
  //inputHandler->processEvent(event);
  switch (event.type) {
    case ALLEGRO_EVENT_DISPLAY_RESIZE:
      LOG(TAG_,"resize gui \n");
      gui->resizeToDisplay();
      break;
    default:
      break;
  }
  
  return NULL;
}


void GuiThread::control_event_handler(UserEvent &event) {

}





void GuiThread::draw() {
  gui->logic();
  al_clear_to_color(al_map_rgb(240,240,240));
  //render the widgets
  gui->render();
}


