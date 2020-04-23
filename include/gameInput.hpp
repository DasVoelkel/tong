#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <vector>

#include <helper.hpp>
#include <types.hpp>

namespace game_input
{

#define KEY_SEEN 1
#define KEY_RELEASED 2

void start(ALLEGRO_EVENT_SOURCE *event_source);
void stop();

ALLEGRO_THREAD *get_p_input_thread();

} // namespace game_input