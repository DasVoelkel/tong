#include <rendering.hpp>

#include <WidgetCreator.hpp>
#include <types.hpp>
namespace rendering
{
  size_t display_width = 0;
  size_t display_height = 0;

  agui::Gui *gui = NULL;
  agui::Allegro5Input *inputHandler = NULL;
  agui::Allegro5Graphics *graphicsHandler = NULL;
  agui::Font *defaultFont = NULL;

  WidgetCreator *creator = NULL;

  void rendergui();

  void initializeAgui()
  {

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

    fprintf(stderr, al_get_current_directory());
    fprintf(stderr, "\n");

    if (al_filename_exists("assets/DejaVuSans.ttf"))
      fprintf(stderr, "present \n");
    else
      fprintf(stderr, "not present \n");

    auto font = al_load_font("assets/DejaVuSans.ttf", 16, agui::FONT_DEFAULT_FLAGS);
    fprintf(stderr, "step \n");

    font = al_load_ttf_font("assets/DejaVuSans.ttf", 16, 1);
    fprintf(stderr, "step \n");

    defaultFont = agui::Font::load("assets/DejaVuSans.ttf", 16);

    //Setting a global font is required and failure to do so will crash.
    agui::Widget::setGlobalFont(defaultFont);
  }

  void render(ALLEGRO_DISPLAY *disp, RENDER_SCENES scene)
  {
    {
      if (!gui)
      {
        initializeAgui();
        rendergui();
      }
    }
    {
      std::string title(get_game_title());
      title += " - ";
      title += repr(scene);
      al_set_window_title(disp, title.c_str());
    }
    display_width = al_get_display_height(disp);
    display_height = al_get_display_width(disp);

    switch (scene)
    {
    case RENDER_SCENES::R_M_MAIN:
      break;
    case RENDER_SCENES::R_M_OPTIONS:
      break;
    case RENDER_SCENES::R_M_LOBBY:
      break;
    case RENDER_SCENES::R_GAME:
      break;

    default:
      fprintf(stderr, "unknown state");

      break;
    }
  }

  void addWidgets()
  {
    creator = new WidgetCreator(gui);
  }

  void cleanUp() // TODO call this on exit
  {
    if (gui)
    {

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
  }

  void rendergui()
  {
    al_clear_to_color(al_map_rgb(240, 240, 240));

    //render the widgets
    gui->render();
  }
} // namespace rendering