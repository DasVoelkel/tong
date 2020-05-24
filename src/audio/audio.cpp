#include <audio/audio.hpp>

ALLEGRO_SAMPLE *sample_shot;
ALLEGRO_SAMPLE *sample_explode[2];

void audio_init()
{
  al_install_audio();
  al_init_acodec_addon();
  //al_reserve_samples(128);
}

void audio_deinit()
{
  /* al_destroy_sample(sample_shot);
  al_destroy_sample(sample_explode[0]);
  al_destroy_sample(sample_explode[1]);*/
}
