#pragma once

#include <types.hpp>

class GameAssetAnimation
{
private:
    std::string _name;

public:
    std::vector<ALLEGRO_BITMAP *> _sprites;

    GameAssetAnimation(char *name) : _name(name)
    {
    }
    ~GameAssetAnimation()
    {
        for (auto it = _sprites.begin(); it != _sprites.end(); it++)
            al_destroy_bitmap(*it);
    }

    void add_sprite(ALLEGRO_BITMAP *sprite);
    bool draw(ALLEGRO_DISPLAY *disp, point coords, int frame_counter);

    int get_height();
    int get_width();
    int get_sprite_size();
};