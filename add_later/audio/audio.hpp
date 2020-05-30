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
#include <core/helper.hpp>

// --- audio ---

extern ALLEGRO_SAMPLE *sample_shot;
extern ALLEGRO_SAMPLE *sample_explode[2];

void audio_init();
void audio_deinit();