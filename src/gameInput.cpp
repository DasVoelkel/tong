#include <gameInput.hpp>

unsigned char key[ALLEGRO_KEY_MAX];
ALLEGRO_EVENT_QUEUE *event_queue_input_thread = NULL;

void keyboard_init()
{
    must_init(al_install_keyboard(), "keyboard");
    memset(key, 0, sizeof(key));
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
}

void keyboard_seen()
{
    for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
        key[i] &= KEY_SEEN;
}

ALLEGRO_THREAD *keyboard_input_thread_init()
{
    event_queue_input_thread = al_create_event_queue();
    must_init(event_queue_input_thread, "draw-thread-queue");

    al_register_event_source(event_queue_input_thread, al_get_keyboard_event_source());
    al_register_event_source(event_queue_input_thread, &game_state_event_source); // Gstate informant
    al_register_event_source(event_queue_input_thread, al_get_display_event_source(disp));

    ALLEGRO_THREAD *ret = al_create_thread(input_thread, NULL);
    if (ret)
    {
        //al_set_thread_should_stop(ret);
        al_start_thread(ret);
        return ret;
    }
    else
    {
        return NULL;
    }
}

void input_thread_deinit()
{

    al_destroy_event_queue(event_queue_input_thread);
    fprintf(stderr, "input thread deinit\n");
}

void *input_thread(ALLEGRO_THREAD *thr, void *arg)
{
    fprintf(stderr, "input thread started\n");
    ALLEGRO_EVENT event;

    while (g_state != D_EXIT)
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

            fprintf(stderr, "input thread got gamestate change %i \n", g_state);

            break;

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            g_state_update_event(D_EXIT);
            break;
        default:
            fprintf(stderr, "unexpected event in input thread! >%i<\n", event.type);
            break;
        }
    }
    fprintf(stderr, "input thread exit\n");
}