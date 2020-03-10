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

enum OBJ_TYPE
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

enum PLAYING_TYPE
{
    P_SPAWN,
    P_STATIONARY,
    P_DEATH

};

struct point
{
    int x, y;
};

enum GAME_STATES
{
    D_MENU,
    D_GAME,
    D_OPTIONS,
    D_EXIT
};