#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

const int MAX_OBJ_NAME_LENGTH = 32;

typedef enum OBJ_TYPE
{
    // general
    gameborder,

    //Space invaders
    space_ship,
    alien_bug,
    alien_arrow,
    alien_boss,

    // Tetris
    block_tetris,
    block_l_left,
    block_l_right,
    block_2x2,
    block_stairs_left,
    block_stairs_right,
    block_pyramid
};

typedef enum PLAYING_TYPE
{
    P_SPAWN,
    P_STATIONARY,
    P_DEATH

};

typedef struct point
{
    int x, y;
};

typedef class Animation
{
private:
    std::string _name;

public:
    std::vector<ALLEGRO_BITMAP *> _sprites;

    Animation(char *name) : _name(name)
    {
    }
    ~Animation()
    {
        for (auto it = _sprites.begin(); it != _sprites.end(); it++)
            al_destroy_bitmap(*it);
    }

    void add_sprite(ALLEGRO_BITMAP *sprite)
    {
        _sprites.push_back(sprite);
    }

    int get_sprite_size()
    {
        return _sprites.size();
    }
    bool draw(ALLEGRO_DISPLAY *disp, point coords, int frame_counter)
    {
        if (_sprites.size == 0)
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

    int get_height()
    {
        return al_get_bitmap_height(_sprites[1]);
    }
    int get_width()
    {
        return al_get_bitmap_width(_sprites[1]);
    }
};

typedef class GameObject
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

    Animation *_sprite_spawn = NULL;
    Animation *_sprite_stationary = NULL;
    Animation *_sprite_death = NULL;

    std::vector<point> _collision_framework;
    point _coordinates = {};

    int _blink = 0;

    GameObject(Animation *sprite_spawn, Animation *sprite_stat, Animation *sprite_death) : _sprite_spawn(sprite_spawn), _sprite_stationary(sprite_stat), _sprite_death(sprite_death)
    {

        if (!_sprite_stationary)
        {
            fprintf(stderr, "No stationary sprite! ", _name);
            assert(false);
        }

        if (_sprite_spawn)
        {
            if (_sprite_spawn->get_sprite_size() < 2)
            {
                fprintf(stderr, "Spawn sprite has less than 2 frames! ", _name);
                assert(false);
            }
            _playing = P_SPAWN;
        }

        if (_sprite_death)
        {
            if (_sprite_death->get_sprite_size() < 2)
            {
                fprintf(stderr, "death sprite has less than 2 frames! ", _name);
                assert(false);
            }
        }
    }

    void reset()
    {
        bool _enable = true;

        PLAYING_TYPE _playing = P_SPAWN;
        int _frame_counter = 0;
    }

    bool draw(ALLEGRO_DISPLAY *disp)
    {
        if (!_enable)
        {
            return false;
        }

        switch (_playing)
        {
        case P_SPAWN:

            if (_sprite_spawn->draw(disp, _coordinates, _frame_counter))
            {
                _frame_counter++;
            }
            else
            {
                _enable = false;
                return false;
            }

            if (_frame_counter > _sprite_spawn->get_sprite_size())
            {
                _playing = P_STATIONARY;
                _frame_counter = 0;
            }

            break;
        case P_STATIONARY:

            if (_sprite_stationary->draw(disp, _coordinates, _frame_counter))
            {
                _frame_counter++;
            }
            else
            {
                _enable = false;
                return false;
            }

            if (_frame_counter > _sprite_stationary->get_sprite_size())
            {
                _frame_counter = 0;
            }
            break;
        case P_DEATH:

            if (_sprite_death->draw(disp, _coordinates, _frame_counter))
            {
                _frame_counter++;
            }
            else
            {
                _enable = false;
                return false;
            }

            if (_frame_counter > _sprite_death->get_sprite_size())
            {
                _frame_counter = 0;
                _enable = false;
            }
            break;
        }

        return true;
    }

    int get_height()
    {
        return _sprite_stationary[1].get_height;
    }
    int get_width()
    {
        _sprite_stationary[1].get_width;
    }

    bool collision_with(GameObject obj)
    {

        if (_coordinates.x > obj._coordinates.y + obj.get_height())
            return false;
        if (_coordinates.x + get_width() < obj._coordinates.x)
            return false;
        if (_coordinates.y > obj._coordinates.x + obj.get_width())
            return false;
        if (_coordinates.y + get_height() < obj._coordinates.y)
            return false;

        return true;
    }
};
