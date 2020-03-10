#pragma once

#include <GameAssetCommon.hpp>
#include <GameAssetAnimation.hpp>

const int MAX_OBJ_NAME_LENGTH = 32;

class GameAsset
{
private:
    std::string _name;

public:
    bool _enable = true;

    PLAYING_TYPE _playing = P_STATIONARY;
    int _frame_counter = 0;

    ALLEGRO_SAMPLE *_sound_on_spawn = NULL;
    ALLEGRO_SAMPLE *_sound_stationary = NULL;
    ALLEGRO_SAMPLE *_sound_on_death = NULL;

    GameAssetAnimation *_sprite_spawn = NULL;
    GameAssetAnimation *_sprite_stationary = NULL;
    GameAssetAnimation *_sprite_death = NULL;

    std::vector<point> _collision_framework;
    point _coordinates = {};

    GameAsset(GameAssetAnimation *sprite_spawn, GameAssetAnimation *sprite_stat, GameAssetAnimation *sprite_death);

    void reset();

    bool draw(ALLEGRO_DISPLAY *disp);

    int get_height();
    int get_width();

    bool collision_with(GameAsset &obj);
};
