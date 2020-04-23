#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_ttf.h>
#include <helper.hpp>

#include <settings.hpp>
#include <GameAssetCommon.hpp>
#include <audio.hpp>
#include <gameInput.hpp>
#include <gameDrawing.hpp>

#define BUFFER_W 480
#define BUFFER_H 240

#define DISP_SCALE 3
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

// event for gstate changes
#define G_STATE_CHANGE_EVENT_NUM 200
// not et used
extern size_t frames;
extern size_t score;

// Program state
extern THREAD_STATES program_state;

// internal font from allegro, used all the time
extern ALLEGRO_FONT *internal_font;

// custom event to inform all threads they are getting closed
extern ALLEGRO_EVENT_SOURCE game_state_event_source;

void addons_init();

/// addons
void init();
void deinit();
void create_display();
void destroy_display();
void create_input();
void destroy_input();

// send g-state update
void g_state_update_event(THREAD_STATES _g_state);