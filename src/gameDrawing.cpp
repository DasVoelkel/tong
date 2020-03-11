#include <gameDrawing.hpp>

ALLEGRO_EVENT_QUEUE *event_queue_draw_thread = NULL;
ALLEGRO_TIMER *timer_draw_thread = NULL;

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    al_set_target_backbuffer(disp);
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, 0, 0, DISP_W, DISP_H, 0);

    al_flip_display();
}

void draw_thread_deinit()
{

    al_destroy_timer(timer_draw_thread);
    al_destroy_event_queue(event_queue_draw_thread);
    fprintf(stderr, "Draw thread deinit\n");
}

ALLEGRO_THREAD *draw_thread_init()
{
    event_queue_draw_thread = al_create_event_queue();
    must_init(event_queue_draw_thread, "draw-thread-queue");

    timer_draw_thread = al_create_timer(1.0 / 60.0);
    must_init(timer_draw_thread, "draw-thread-timer");

    al_register_event_source(event_queue_draw_thread, al_get_timer_event_source(timer_draw_thread));
    al_register_event_source(event_queue_draw_thread, &game_state_event_source); // Gstate informant

    ALLEGRO_THREAD *ret = al_create_thread(draw_thread, NULL);
    if (ret)
    {
        //al_set_thread_should_stop(ret);
        al_start_thread(ret);
        al_start_timer(timer_draw_thread);

        return ret;
    }
    else
    {
        return NULL;
    }
}

void *draw_thread(ALLEGRO_THREAD *thr, void *arg)
{
    fprintf(stderr, "Draw thread started\n");
    ALLEGRO_EVENT event;

    if (g_state == D_RESTART)
    {
        fprintf(stderr, "started draw thread from restart waiting for go ahead \n");
        while (g_state == D_RESTART)
        {
            al_wait_for_event(event_queue_draw_thread, &event);
        }
    }

    while (g_state != D_EXIT && g_state != D_RESTART)
    {

        al_wait_for_event(event_queue_draw_thread, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:

            disp_pre_draw();
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(
                internal_font,
                al_map_rgb_f(1, 1, 1),
                BUFFER_W / 2, BUFFER_H / 2,
                ALLEGRO_ALIGN_CENTER,
                "draw thread started");

            disp_post_draw();
            break;
        case G_STATE_CHANGE_EVENT_NUM:
            // check if we need to close the thread
            fprintf(stderr, "drawing thread got gamestate change %i \n", g_state);

            break;
        default:
            fprintf(stderr, "unexpected event in draw thread!>%i<\n", event.type);
            break;
        }
    }
    fprintf(stderr, "drawing thread exit\n");
}