#include <gameDrawing.hpp>

ALLEGRO_EVENT_QUEUE *event_queue_draw_thread = NULL;
ALLEGRO_EVENT_QUEUE *event_queue_display = NULL;

// timer for fps, thread to do that drawing
ALLEGRO_TIMER *timer_draw_thread = NULL;
ALLEGRO_THREAD *p_draw_thread = NULL;

//we switch from buffer to display when buffer is drawn
ALLEGRO_DISPLAY *disp = NULL;
ALLEGRO_BITMAP *buffer = NULL;

// for scaling
ALLEGRO_TRANSFORM transform;
float scale_factor_x;
float scale_factor_y;
bool fullscreen_mode = false;

ALLEGRO_DISPLAY *
get_disp()
{
    return disp;
}

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

    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
    al_destroy_event_queue(event_queue_display);

    fprintf(stderr, "Draw thread deinit\n");
}

ALLEGRO_THREAD *get_p_draw_thread()
{
    return p_draw_thread;
}

bool draw_thread_init()
{

    // DISP INIT
    if (fullscreen_mode)
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    disp = al_create_display(DISP_W, DISP_H);
    must_init(disp, "display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");

    scale_factor_x = ((float)al_get_display_width(disp)) / BUFFER_W;
    scale_factor_y = ((float)al_get_display_height(disp)) / BUFFER_H;
    if (fullscreen_mode)
    {

        al_set_target_bitmap(buffer);
        al_identity_transform(&transform);
        al_scale_transform(&transform, scale_factor_x, scale_factor_y);
        al_use_transform(&transform);
    }

    // event for display actions
    event_queue_display = al_create_event_queue();
    must_init(event_queue_display, "display_event_queue");
    al_register_event_source(event_queue_display, al_get_display_event_source(disp));

    // THREAD INIT
    event_queue_draw_thread = al_create_event_queue();
    must_init(event_queue_draw_thread, "draw-thread-queue");

    timer_draw_thread = al_create_timer(1.0 / 60.0);
    must_init(timer_draw_thread, "draw-thread-timer");

    al_register_event_source(event_queue_draw_thread, al_get_timer_event_source(timer_draw_thread));
    al_register_event_source(event_queue_draw_thread, &game_state_event_source); // Gstate informant

    p_draw_thread = al_create_thread(draw_thread, NULL);
    if (p_draw_thread)
    {
        //al_set_thread_should_stop(ret);
        al_start_thread(p_draw_thread);
        al_start_timer(timer_draw_thread);

        return true;
    }
    else
    {
        p_draw_thread = NULL;
        return false;
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
        fprintf(stderr, "Go ahead granted: draw thread \n");
    }

    while (g_state != D_EXIT && g_state != D_RESTART)
    {

        al_wait_for_event(event_queue_draw_thread, &event);

        switch (event.type)
        {
        case ALLEGRO_EVENT_TIMER:

            disp_pre_draw();
            al_clear_to_color(al_color_name("black"));
            al_draw_text(internal_font, al_color_name("white"), 1, 1, ALLEGRO_ALIGN_CENTER, "Hello world!");

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