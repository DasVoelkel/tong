#include "core/types.hpp"

ALLEGRO_FONT *internal_font = NULL;

ALLEGRO_FONT *get_font()
{
  if (internal_font)
    return internal_font;
  else
  {
    internal_font = al_create_builtin_font();
    assert(internal_font && "internal font");

    return internal_font;
  }
}

#define BG_COLOR_NAME "black"
ALLEGRO_COLOR *bg_color_p = NULL;
ALLEGRO_COLOR get_background_color()
{
  if (bg_color_p)
    return *bg_color_p;
  else
  {
    bg_color_p = new ALLEGRO_COLOR;
    *bg_color_p = al_color_name(BG_COLOR_NAME);
    return *bg_color_p;
  }
}

const char *repr(RENDER_SCENES target)
{
  switch (target)
  {
  case RENDER_SCENES::R_M_MAIN:
    return "R_M_MAIN";
    break;
  case RENDER_SCENES::R_M_OPTIONS:
    return "R_M_OPTIONS";
    break;
  case RENDER_SCENES::R_M_LOBBY:
    return "R_M_LOBBY";
    break;
  case RENDER_SCENES::R_GAME:
    return "R_GAME";
    break;

  default:
    return "unknown state";
    break;
  }
}
