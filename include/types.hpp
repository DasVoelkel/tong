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
#include <allegro5/allegro_color.h>

// essential
#define G_STATE_CHANGE_EVENT_NUM 200

enum class OBJ_TYPE
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

enum class PLAYING_TYPE
{
    P_SPAWN,
    P_STATIONARY,
    P_DEATH

};

struct point
{
    int x, y;
};

enum class THREAD_STATES
{
    D_STARTING,
    D_RUNNING,
    D_RESTART, // turn this into "starting"!
    D_EXIT
};

enum class RENDER_SCENES
{
    R_M_MAIN,
    R_M_OPTIONS,
    R_M_LOBBY,
    R_GAME
};

const char *repr(THREAD_STATES state);
const char *repr(RENDER_SCENES target);
#define BG_COLOR_NAME "black"
#define GAME_TITLE "Pong"

ALLEGRO_FONT *get_font();
ALLEGRO_COLOR get_background_color();
const char *get_game_title();

THREAD_STATES get_program_state();
void update_program_state(THREAD_STATES new_program_state);
/// end
