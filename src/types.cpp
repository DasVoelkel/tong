#include "types.hpp"

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

ALLEGRO_COLOR bg_color;
ALLEGRO_COLOR *bg_color_p = NULL;
ALLEGRO_COLOR get_background_color()
{
    if (bg_color_p)
        return bg_color;
    else
    {
        bg_color = al_color_name(BG_COLOR_NAME);
        bg_color_p = &bg_color;
    }
}