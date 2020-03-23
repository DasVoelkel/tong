#include <init.hpp>

// --- general
size_t frames = 0;
size_t score = 0;
GAME_STATES g_state = D_RUNNING;

ALLEGRO_FONT *internal_font = NULL;

ALLEGRO_EVENT_SOURCE game_state_event_source;

void init()
{
    must_init(al_init(), "allegro");

    addons_init();
    //audio_init(); // TODO change this so audio can also be reinited, may not be neccessary
    keyboard_init();

    internal_font = al_create_builtin_font();
    must_init(internal_font, "font_init_buildin");
    al_init_user_event_source(&game_state_event_source);

    create_display(); // FIRST DISPLAY  ! we need the frame for input management
    create_input();
}

void create_display()
{

    must_init(draw_thread_init(), "create display: draw thread init");
    fprintf(stderr, "created display + draw thread\n");
}

void destroy_display()
{
    al_join_thread(get_p_draw_thread(), NULL);
    draw_thread_deinit();
}

void create_input()
{
    must_init(keyboard_input_thread_init(), "keyboard create");
}

void destroy_input()
{
    al_join_thread(get_p_input_thread(), NULL);
    input_thread_deinit();
}

void deinit()
{

    destroy_input();
    destroy_display();

    al_destroy_user_event_source(&game_state_event_source);
    al_destroy_font(internal_font);
    audio_deinit();
}

void addons_init()
{
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    //must_init(al_install_audio(), "audio");
    //must_init(al_init_acodec_addon(), "audio codecs");
    //must_init(al_reserve_samples(16), "reserve samples");
}

/// used by all TODO maybe move

void g_state_update_event(GAME_STATES _g_state)
{
    ALLEGRO_EVENT g_state_event;
    g_state_event.type = G_STATE_CHANGE_EVENT_NUM;
    g_state = _g_state;
    if (al_emit_user_event(&game_state_event_source, &g_state_event, NULL))
    {
        fprintf(stderr, "success sending event change! \n ");
    }
    else
    {
        fprintf(stderr, "fail sending event change! \n");
        assert(false);
    }
}