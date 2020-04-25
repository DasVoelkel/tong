#include <rendering.hpp>

namespace rendering
{
size_t display_width = 0;
size_t display_height = 0;

void render(ALLEGRO_DISPLAY *disp, RENDER_SCENES scene)
{
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

} // namespace rendering