#include <nlohmann/json.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include <GameAsset.hpp>
// settings required
#define DEFAULT_SETTINGS \
    "{\"settings\":{\"fullscreen\":false,\"scale_fac\":3,\"anti_a\":5,\"audio_volume\":100}}"
#define OPT_NAME "options.json"

bool read_options();
bool save_opt();
nlohmann::json *get_options();