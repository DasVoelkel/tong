#include <settings.hpp>
// ---- options
nlohmann::json json_buffer;
nlohmann::json *json_buffer_p = NULL;

nlohmann::json get_default_empty()
{
    nlohmann::json ret;

    ret = nlohmann::json::parse(DEFAULT_SETTINGS);
    return ret;
}

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

nlohmann::json *get_options()
{
    if (json_buffer_p)
        return json_buffer_p;
    else if (read_options)
    {
        return json_buffer_p;
    }
    else
    {
        fprintf(stderr, "can'T read opt \n");
        assert(false);
        return NULL;
    }
}
void set_opt(nlohmann::json opt)
{
    json_buffer = opt;
    json_buffer_p = &json_buffer;
}

void clear_opt()
{
    json_buffer = nlohmann::json({});
    json_buffer_p = NULL;
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

bool read_options()
{
    nlohmann::json opt_j = {};

    std::fstream opt_fs;

    if (opt_valid())
    {
        opt_fs.open(OPT_NAME, std::ios::in);
        opt_fs >> opt_j;
        opt_fs.close();

        fprintf(stderr, "returned opt file scanned \n");
        set_opt(opt_j);
        return true;
    }
    else
    {
        if (create_default_opt())
        {
            opt_fs.open(OPT_NAME, std::ios::in);
            opt_fs >> opt_j;
            opt_fs.close();

            fprintf(stderr, "returned opt file default \n");
            set_opt(opt_j);

            return true;
        }
        else
        {
            fprintf(stderr, "Invalid opt file, can't create default \n");
            clear_opt();
            return false;
        }
    }
    assert(false);
}

bool save_opt()
{
    if (json_buffer_p)
    {
        std::fstream opt_fs;
        opt_fs.open(OPT_NAME, std::ios::out | std::ofstream::trunc);

        if (!opt_fs.is_open())
        {
            opt_fs.close();
            fprintf(stderr, "Can't open opt for saving\n");
            return false;
        }
        opt_fs << std::setw(4) << json_buffer << std::endl;
        opt_fs.close();
        fprintf(stderr, "Saved data\n");
        return true;
    }
    else
    {
        fprintf(stderr, "No settings open, can't save!\n");
        return false;
    }

    assert(false); // should not happen
}

