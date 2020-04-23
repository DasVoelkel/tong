/* this is a complete copy of the source from allegro vivace's 'gameplay' section.
 *
 * for gcc users, it can be compiled & run with:
 *
 * gcc game.c -o game $(pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 --libs --cflags)
 * ./game
 */

#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>

#include <helper.hpp>
#include <init.hpp>
#include <audio.hpp>
#include <sprites.hpp>
#include <gameInput.hpp>

// --- general ---

// --- sprites ---

#define SHIP_W 12
#define SHIP_H 13

#define SHIP_SHOT_W 2
#define SHIP_SHOT_H 9

#define LIFE_W 6
#define LIFE_H 6

const int ALIEN_W[] = {14, 13, 45};
const int ALIEN_H[] = {9, 10, 27};

#define ALIEN_BUG_W ALIEN_W[0]
#define ALIEN_BUG_H ALIEN_H[0]
#define ALIEN_ARROW_W ALIEN_W[1]
#define ALIEN_ARROW_H ALIEN_H[1]
#define ALIEN_THICCBOI_W ALIEN_W[2]
#define ALIEN_THICCBOI_H ALIEN_H[2]

#define ALIEN_SHOT_W 4
#define ALIEN_SHOT_H 4

#define EXPLOSION_FRAMES 4
#define SPARKS_FRAMES 3

typedef struct SPRITES
{
    ALLEGRO_BITMAP *_sheet;

    ALLEGRO_BITMAP *ship;
    ALLEGRO_BITMAP *ship_shot[2];
    ALLEGRO_BITMAP *life;

    ALLEGRO_BITMAP *alien[3];
    ALLEGRO_BITMAP *alien_shot;

    ALLEGRO_BITMAP *explosion[EXPLOSION_FRAMES];
    ALLEGRO_BITMAP *sparks[SPARKS_FRAMES];

    ALLEGRO_BITMAP *powerup[4];
} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP *sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP *sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("spritesheet.png");
    must_init(sprites._sheet, "spritesheet");

    sprites.ship = sprite_grab(0, 0, SHIP_W, SHIP_H);

    sprites.life = sprite_grab(0, 14, LIFE_W, LIFE_H);

    sprites.ship_shot[0] = sprite_grab(13, 0, SHIP_SHOT_W, SHIP_SHOT_H);
    sprites.ship_shot[1] = sprite_grab(16, 0, SHIP_SHOT_W, SHIP_SHOT_H);

    sprites.alien[0] = sprite_grab(19, 0, ALIEN_BUG_W, ALIEN_BUG_H);
    sprites.alien[1] = sprite_grab(19, 10, ALIEN_ARROW_W, ALIEN_ARROW_H);
    sprites.alien[2] = sprite_grab(0, 21, ALIEN_THICCBOI_W, ALIEN_THICCBOI_H);

    sprites.alien_shot = sprite_grab(13, 10, ALIEN_SHOT_W, ALIEN_SHOT_H);

    sprites.explosion[0] = sprite_grab(33, 10, 9, 9);
    sprites.explosion[1] = sprite_grab(43, 9, 11, 11);
    sprites.explosion[2] = sprite_grab(46, 21, 17, 18);
    sprites.explosion[3] = sprite_grab(46, 40, 17, 17);

    sprites.sparks[0] = sprite_grab(34, 0, 10, 8);
    sprites.sparks[1] = sprite_grab(45, 0, 7, 8);
    sprites.sparks[2] = sprite_grab(54, 0, 9, 8);

    sprites.powerup[0] = sprite_grab(0, 49, 9, 12);
    sprites.powerup[1] = sprite_grab(10, 49, 9, 12);
    sprites.powerup[2] = sprite_grab(20, 49, 9, 12);
    sprites.powerup[3] = sprite_grab(30, 49, 9, 12);
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.ship);

    al_destroy_bitmap(sprites.ship_shot[0]);
    al_destroy_bitmap(sprites.ship_shot[1]);

    al_destroy_bitmap(sprites.alien[0]);
    al_destroy_bitmap(sprites.alien[1]);
    al_destroy_bitmap(sprites.alien[2]);

    al_destroy_bitmap(sprites.sparks[0]);
    al_destroy_bitmap(sprites.sparks[1]);
    al_destroy_bitmap(sprites.sparks[2]);

    al_destroy_bitmap(sprites.explosion[0]);
    al_destroy_bitmap(sprites.explosion[1]);
    al_destroy_bitmap(sprites.explosion[2]);
    al_destroy_bitmap(sprites.explosion[3]);

    al_destroy_bitmap(sprites.powerup[0]);
    al_destroy_bitmap(sprites.powerup[1]);
    al_destroy_bitmap(sprites.powerup[2]);
    al_destroy_bitmap(sprites.powerup[3]);

    al_destroy_bitmap(sprites._sheet);
}

// --- fx ---

// --- main ---

int main() // MAIN IS OUR CONTROL THREAD
{

    init();

    ALLEGRO_EVENT_QUEUE *event_queue_control_thread = al_create_event_queue();
    must_init(event_queue_control_thread, "event-queue-control-thread");
    al_register_event_source(event_queue_control_thread, &game_state_event_source);

    ALLEGRO_EVENT event;

    while (program_state != D_EXIT)
    {
        al_wait_for_event(event_queue_control_thread, &event);

        switch (program_state)
        {
        case D_EXIT:
            break;

        case D_RESTART:
            fprintf(stderr, "Control saw draw thread restart\n");
            // close all threads, then restart them and recreate disp first !
            destroy_input();
            destroy_display();

            create_input();
            create_display();
            g_state_update_event(D_RUNNING);

            break;

        default:
            // main button processing

            break;
        }
    }

    deinit();

    exit(0);
}
