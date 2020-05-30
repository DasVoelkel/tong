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

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

// essential
#define G_STATE_CHANGE_EVENT_NUM 200

enum class RENDER_SCENES
{
  R_M_MAIN,
  R_M_OPTIONS,
  R_M_LOBBY,
  R_GAME
};

const char *repr(RENDER_SCENES target);

ALLEGRO_FONT *get_font();
ALLEGRO_COLOR get_background_color();
const char *get_game_title();

/// end
