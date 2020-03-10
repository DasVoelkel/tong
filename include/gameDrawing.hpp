#include <iostream>
#include <cstdlib>
#include <vector>

#include <helper.hpp>
#include <init.hpp>
#include <GameAssetCommon.hpp>

//extern std::vector<GameAsset> gameAssetsList;

extern ALLEGRO_EVENT_QUEUE *event_queue_draw_thread;

ALLEGRO_THREAD *draw_thread_init();
void draw_thread_deinit();

void *draw_thread(ALLEGRO_THREAD *thr, void *arg);

void disp_pre_draw();

void disp_post_draw();