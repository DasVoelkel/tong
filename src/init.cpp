#include <init.hpp>

// ---- general
size_t frames = 0;
size_t score = 0;
GAME_STATES g_state = D_MENU;

ALLEGRO_DISPLAY *disp;
ALLEGRO_BITMAP *buffer;

ALLEGRO_THREAD *p_draw_thread;
ALLEGRO_THREAD *p_input_thread;

ALLEGRO_EVENT_QUEUE *event_queue_display = NULL;

ALLEGRO_FONT *internal_font = NULL;

ALLEGRO_EVENT_SOURCE game_state_event_source;

void init()
{
    addons_init();
    disp_init();
    audio_init();
    keyboard_init();

    internal_font = al_create_builtin_font();
    must_init(internal_font, "font_init_buildin");

    al_init_user_event_source(&game_state_event_source);

    p_draw_thread = draw_thread_init();
    p_input_thread = keyboard_input_thread_init();
}

void deinit()
{
    al_join_thread(p_input_thread, NULL);
    al_join_thread(p_draw_thread, NULL);

    draw_thread_deinit();
    input_thread_deinit();
    al_destroy_user_event_source(&game_state_event_source);
    al_destroy_font(internal_font);

   
    audio_deinit();
    disp_deinit();
}

void disp_init()
{
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");

    event_queue_display = al_create_event_queue();
    must_init(event_queue_display, "display_event_queue");
    al_register_event_source(event_queue_display, al_get_display_event_source(disp));
}

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
    al_destroy_event_queue(event_queue_display);
}

void addons_init()
{
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    must_init(al_reserve_samples(16), "reserve samples");
}

// ---- options

bool opt_file_exists(const char *name)
{
    std::fstream opt_fs;
    opt_fs.open(name, std::ios::in);

    if (!opt_fs.is_open())
    {

        fprintf(stderr, "Can't open opt file! \n");
        opt_fs.close();
        return false;
    }
    opt_fs.close();
    fprintf(stderr, "Opt file present \n");

    return true;
}

bool opt_valid()
{
    if (!opt_file_exists(OPT_NAME))
        return false;

    nlohmann::json opt_j = {};
    std::fstream opt_fs;
    opt_fs.open(OPT_NAME, std::ios::in);
    try
    {
        opt_fs >> opt_j;
    }
    catch (...)
    {
        return false;
    }
    return true;
}

nlohmann::json read_options()
{
    nlohmann::json opt_j = {};

    std::fstream opt_fs;
    if (opt_valid())
    {
        opt_fs.open(OPT_NAME, std::ios::in);
        opt_fs >> opt_j;
        opt_fs.close();

        fprintf(stderr, "returned opt file \n");

        return opt_j;
    }
    else
    {
        if (create_default_opt())
        {
            opt_fs.open(OPT_NAME, std::ios::in);
            opt_fs >> opt_j;
            opt_fs.close();

            fprintf(stderr, "returned opt file \n");

            return opt_j;
        }
        else
        {
            fprintf(stderr, "Invalid opt file, can't create default \n");
            return nlohmann::json({});
        }
    }
    assert(false);
}

bool create_default_opt()
{
    std::fstream opt_fs;
    opt_fs.open(OPT_NAME, std::ios::out | std::ofstream::trunc);

    if (!opt_fs.is_open())
    {
        opt_fs.close();
        fprintf(stderr, "Can't default opt create file! \n");
        return false;
    }
    opt_fs << std::setw(4) << get_default_empty() << std::endl;
    opt_fs.close();
    return true;
}

bool save_opt(nlohmann::json data)
{
    std::fstream opt_fs;
    opt_fs.open(OPT_NAME, std::ios::out | std::ofstream::trunc);

    if (!opt_fs.is_open())
    {
        opt_fs.close();
        fprintf(stderr, "Can't open opt for saving\n");
        return false;
    }
    opt_fs << std::setw(4) << data << std::endl;
    opt_fs.close();
    fprintf(stderr, "Saved data\n");

    return true;
}
nlohmann::json get_default_empty()
{
    return {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {{"everything", 42}}},
        {"list", {1, 0, 2}},
        {"object", {{"currency", "USD"}, {"value", 42.99}}}

    };
}

/// init essentials

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
