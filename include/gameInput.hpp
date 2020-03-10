#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>

#include <helper.hpp>
#include <init.hpp>
#include <GameAssetCommon.hpp>
#define KEY_SEEN 1
#define KEY_RELEASED 2

extern unsigned char key[ALLEGRO_KEY_MAX];
extern ALLEGRO_EVENT_QUEUE *event_queue_input_thread;

void keyboard_init();
void input_thread_deinit();

void keyboard_seen();

ALLEGRO_THREAD *keyboard_input_thread_init();
void *input_thread(ALLEGRO_THREAD *thr, void *arg);
