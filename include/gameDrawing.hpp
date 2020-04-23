#include <iostream>
#include <cstdlib>
#include <vector>

#include <allegro5/allegro_color.h>

#include <types.hpp>
#include <helper.hpp>
#include <settings.hpp>

namespace game_display_output
{

#define BUFFER_W 480
#define BUFFER_H 240

#define DISP_SCALE 3
#define DISP_W (BUFFER_W * DISP_SCALE)
#define DISP_H (BUFFER_H * DISP_SCALE)

// not et used
extern size_t frames;

//extern std::vector<GameAsset> gameAssetsList;
bool start(ALLEGRO_EVENT_SOURCE *event_source);
void stop();

ALLEGRO_THREAD *get_p_draw_thread(); // ever used ?
ALLEGRO_DISPLAY *get_disp();

} // namespace game_display_output