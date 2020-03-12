#include <iostream>
#include <cstdlib>
#include <vector>

#include <helper.hpp>
#include <init.hpp>
#include <GameAssetCommon.hpp>

//extern std::vector<GameAsset> gameAssetsList;

bool draw_thread_init();
void draw_thread_deinit();

void *draw_thread(ALLEGRO_THREAD *thr, void *arg);

void disp_pre_draw();

void disp_post_draw();

ALLEGRO_THREAD *get_p_draw_thread();

ALLEGRO_DISPLAY *get_disp();