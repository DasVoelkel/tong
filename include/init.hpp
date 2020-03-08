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
#include <helper.hpp>
// ---- general

extern long frames;
extern long score;

// --- display
#define BUFFER_W 320
#define BUFFER_H 240

#define DISP_SCALE 3
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)
extern ALLEGRO_DISPLAY *disp;
extern ALLEGRO_BITMAP *buffer;

void disp_init();
void disp_deinit();
void disp_pre_draw();
void disp_post_draw();

// --keyboard
#define KEY_SEEN 1
#define KEY_RELEASED 2
extern unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init();
void keyboard_update(ALLEGRO_EVENT *event);