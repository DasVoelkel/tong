#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <helper.hpp>
#include <nlohmann/json.hpp>
#include <iomanip>

#include <GameAssetCommon.hpp>
#include <audio.hpp>
#include <gameInput.hpp>
#include <gameDrawing.hpp>

#define BUFFER_W 480
#define BUFFER_H 240

#define DISP_SCALE 3
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)
#define OPT_NAME "options.json"

// event for gstate changes
#define G_STATE_CHANGE_EVENT_NUM 200
// not et used
extern size_t frames;
extern size_t score;

// control how the game reacts
extern GAME_STATES g_state;

// internal font from allegro, used all the time
extern ALLEGRO_FONT *internal_font;

// custom event to inform all threads they are getting closed
extern ALLEGRO_EVENT_SOURCE game_state_event_source;

void addons_init();

bool read_options();
nlohmann::json *get_options();

nlohmann::json get_default_empty();
bool create_default_opt();
bool opt_file_exists(const char *name);

bool save_opt(nlohmann::json data);

/// addons
void init();
void deinit();
void create_display();
void destroy_display();
void create_input();
void destroy_input();

// send g-state update
void g_state_update_event(GAME_STATES _g_state);