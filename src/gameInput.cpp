#include <gameInput.hpp>
namespace game_input
{

unsigned char key[ALLEGRO_KEY_MAX];
ALLEGRO_EVENT_QUEUE *event_queue_input_thread = NULL;
ALLEGRO_THREAD *p_input_thread = NULL;

//prototypes
bool keyboard_input_thread_init(ALLEGRO_EVENT_SOURCE *control_event_source, ALLEGRO_EVENT_SOURCE *display_event_source);
void process_keys();
void *input_thread(ALLEGRO_THREAD *thr, void *arg);

void start(ALLEGRO_EVENT_SOURCE *event_source, ALLEGRO_EVENT_SOURCE *display_event_source)
{
    memset(key, 0, sizeof(key));
    keyboard_input_thread_init(event_source, display_event_source);
}

void stop()
{

    al_join_thread(get_p_input_thread(), NULL);
    al_destroy_event_queue(event_queue_input_thread);
}

void keyboard_update(ALLEGRO_EVENT *event)
{

    switch (event->type)
    {

    case ALLEGRO_EVENT_KEY_DOWN:
        fprintf(stderr, "Button pressed, input thread \n");
        key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
        break;
    case ALLEGRO_EVENT_KEY_UP:
        fprintf(stderr, "Button released, input thread \n");

        key[event->keyboard.keycode] &= KEY_RELEASED;
        break;
    }
    process_keys();
}

void keyboard_seen()
{
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
        key[i] &= KEY_SEEN;
}

ALLEGRO_THREAD *get_p_input_thread()
{
    return p_input_thread;
}

bool keyboard_input_thread_init(ALLEGRO_EVENT_SOURCE *control_event_source, ALLEGRO_EVENT_SOURCE *display_event_source)
{
    event_queue_input_thread = al_create_event_queue();
    must_init(event_queue_input_thread, "draw-thread-queue");

    al_register_event_source(event_queue_input_thread, al_get_keyboard_event_source());
    al_register_event_source(event_queue_input_thread, control_event_source); // Gstate informant
    must_init(display_event_source, "need initialised disp to get frame input \n");

    al_register_event_source(event_queue_input_thread, display_event_source); // al_get_display_event_source(

    p_input_thread = al_create_thread(input_thread, NULL);
    if (p_input_thread)
    {
        //al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP); // does this do anything?
        al_start_thread(p_input_thread);
        return true;
    }
    else
    {
        p_input_thread = NULL;
        return false;
        ;
    }
}

void *input_thread(ALLEGRO_THREAD *thr, void *arg)
{
    fprintf(stderr, "input thread started\n");
    ALLEGRO_EVENT event;

    if (get_program_state() == THREAD_STATES::D_RESTART)
    {
        fprintf(stderr, "started input thread from restart waiting for go ahead \n");
        while (get_program_state() == THREAD_STATES::D_RESTART)
        {
            al_wait_for_event(event_queue_input_thread, &event);
        }
        fprintf(stderr, "Go ahead granted: input thread \n");
    }

    while (get_program_state() != THREAD_STATES::D_EXIT && get_program_state() != THREAD_STATES::D_RESTART)
    {
        al_wait_for_event(event_queue_input_thread, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_KEY_UP:
            keyboard_update(&event);
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            keyboard_update(&event);
            break;
        case ALLEGRO_EVENT_KEY_CHAR:
            // do nothing
            break;
        case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
            fprintf(stderr, "switched into display\n");

            break;
        case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
            fprintf(stderr, "switched out of display\n");

            break;

        case G_STATE_CHANGE_EVENT_NUM:
            // check if we need to close the thread

            fprintf(stderr, "input thread got gamestate change %i \n", get_program_state());

            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            update_program_state(THREAD_STATES::D_EXIT);
            break;
        default:
            fprintf(stderr, "unexpected event in input thread! >%i<\n", event.type);
            break;
        }
    }
    fprintf(stderr, "input thread exit\n");
    return NULL;
}

void process_keys()
{

    for (int processed_key_id = 0; processed_key_id < ALLEGRO_KEY_MAX; processed_key_id++)
    {

        if (key[processed_key_id])
        {
            switch (processed_key_id)
            {
            case ALLEGRO_KEY_ESCAPE:
                // exit program
                update_program_state(THREAD_STATES::D_EXIT);

                break;

            case ALLEGRO_KEY_R:
                // restart display thread
                update_program_state(THREAD_STATES::D_RESTART);

                break;
            default:
                break;
            }
            fprintf(stderr, "Processed: %s \n", al_keycode_to_name(processed_key_id));
        }

        key[processed_key_id] &= KEY_SEEN;
    }
}
} // namespace game_input