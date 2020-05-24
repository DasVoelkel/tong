#pragma once
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

//#include <GameAsset.hpp>

// settings required
#define DEFAULT_SETTINGS \
  "{\"settings\":{\"fullscreen\":false,\"scale_fac\":3,\"anti_a\":5,\"audio_volume\":100}}"
#define OPT_NAME "options.json"

bool read_options();
bool save_opt();
nlohmann::json *get_options();

// getter
bool get_fullscreen();
size_t get_scale();
size_t get_anti_a();
size_t get_audio_volume();

void set_fullscreen(bool new_val);
void set_scale(size_t new_val);
void set_anti_a(size_t new_val);
void set_audio_volume(size_t new_val);
