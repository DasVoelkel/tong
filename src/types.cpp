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