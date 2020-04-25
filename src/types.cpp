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

const char *repr(THREAD_STATES state)
{
    switch (state)
    {
    case THREAD_STATES::D_STARTING:
        return " D_STARTING ";
        break;
    case THREAD_STATES::D_RUNNING:
        return " D_RUNNING ";
        break;
    case THREAD_STATES::D_RESTART:
        return " D_RESTART ";
        break;
    case THREAD_STATES::D_EXIT:
        return " D_EXIT ";
        break;

    default:
        return "unknown state";
        break;
    }
}
