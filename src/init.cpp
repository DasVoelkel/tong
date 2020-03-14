#include <init.hpp>

// ---- general
size_t frames = 0;
size_t score = 0;
GAME_STATES g_state = D_RUNNING;

ALLEGRO_FONT *internal_font = NULL;

ALLEGRO_EVENT_SOURCE game_state_event_source;

void init()
{
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
    //audio_deinit();
}

void addons_init()
{
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    //must_init(al_install_audio(), "audio");
    must_init(al_init_acodec_addon(), "audio codecs");
    //must_init(al_reserve_samples(16), "reserve samples");
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

    // check if all settings are here, if not, it's invalid

    nlohmann::json compare_value = get_default_empty().at("settings");

    if ((opt_j.find("settings") == opt_j.end()))
    {
        fprintf(stderr, "essential settings missing, no settings at all \n");
        return false;
    }

    for (auto &el : compare_value.items())
    {
        if (!(opt_j.at("settings").find(el.key()) != opt_j.at("settings").end()))
        {
            fprintf(stderr, "essential settings missing \n");
            return false;
        }
    }
    fprintf(stderr, "essential settings present \n");
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

        fprintf(stderr, "returned opt file scanned \n");

        return opt_j;
    }
    else
    {
        if (create_default_opt())
        {
            opt_fs.open(OPT_NAME, std::ios::in);
            opt_fs >> opt_j;
            opt_fs.close();

            fprintf(stderr, "returned opt file default \n");

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

#define DEFAULT_SETTINGS \
    "{\"settings\":{\"fullscreen\":false,\"scale_fac\":3,\"anti_a\":5,\"audio_volume\":100}}"
nlohmann::json get_default_empty()
{
    nlohmann::json ret;

    ret = nlohmann::json::parse(DEFAULT_SETTINGS);
    return ret;
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
