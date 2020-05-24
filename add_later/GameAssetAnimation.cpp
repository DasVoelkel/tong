#include <GameAssetAnimation.hpp>

void GameAssetAnimation::add_sprite(ALLEGRO_BITMAP *sprite)
{
    _sprites.push_back(sprite);
}

int GameAssetAnimation::get_sprite_size()
{
    return _sprites.size();
}
bool GameAssetAnimation::draw(ALLEGRO_DISPLAY *disp, point coords, int frame_counter)
{
    if (_sprites.size() == 0)
    {
        fprintf(stderr, "Animation has no sprites:  %s ", _name);

        return false;
    }

    if (coords.y > al_get_display_height(disp) || coords.y < 0 || coords.x > al_get_display_width(disp) || coords.x < 0)
    {
        fprintf(stderr, "sprite out of display area %s", _name);

        return false;
    }

    al_draw_bitmap(_sprites[frame_counter], coords.x, coords.y, 0);

    return true;
}

int GameAssetAnimation::get_height()
{
    return al_get_bitmap_height(_sprites[1]);
}
int GameAssetAnimation::get_width()
{
    return al_get_bitmap_width(_sprites[1]);
}