#include "main.h"

#define LONG_PRESS_DELAY 300
#define NEXT_STEP_DELAY 300

/**
 * Update the date display text
 */
static void update_date_display()
{
    static char date_buffer[32];
    struct tm *time_info = localtime(&simulation_time);
    snprintf(date_buffer, sizeof(date_buffer), "%04d-%02d-%02d", time_info->tm_year + 1900, time_info->tm_mon + 1,
             time_info->tm_mday);
    text_layer_set_text(date_layer, date_buffer);
}

/**
 * Update the simulation time in the given direction and re-draw the planets
 * @param direction 1 for forward, -1 for backward
 */
static void tick_simulation_time(int direction)
{
    if (direction != 0)
    {
        int64_t delta = (int64_t)direction * time_step_days * 86400;
        int64_t new_time = (int64_t)simulation_time + delta;

        // Clamp to limits if we would overflow
        if (new_time > MAX_TIME_T)
        {
            simulation_time = (time_t)MAX_TIME_T;
        }
        else if (new_time < MIN_TIME_T)
        {
            simulation_time = (time_t)MIN_TIME_T;
        }
        else
        {
            simulation_time = (time_t)new_time;
        }

        struct tm *time_info = localtime(&simulation_time);
        update_planet_positions(time_info);
        update_date_display();
    }
}

/**
 * Timer callback for continuous time stepping
 */
static void step_timer_callback(void *data)
{
    if (step_direction != 0)
    {
        tick_simulation_time(step_direction);
        // Schedule next step
        step_timer = app_timer_register(NEXT_STEP_DELAY, step_timer_callback, NULL);
    }
}

/**
 * Helper function to step time by a number of days in a given direction
 * @param click_count Number of button clicks (1-4+)
 * @param direction 1 for forward, -1 for backward
 */
static void step_time_by_clicks(uint8_t click_count, int direction)
{
    // Set step size based on click count
    if (click_count == 1)
    {
        time_step_days = 1;
    }
    else if (click_count == 2)
    {
        time_step_days = 7;
    }
    else if (click_count == 3)
    {
        time_step_days = 30;
    }
    else
    {
        time_step_days = 365;
    }

    tick_simulation_time(direction);
}

/**
 * UP button multi-click handler
 * 1 click = 1 day, 2 clicks = 7 days, 3 clicks = 30 days, 4+ clicks = 365 days
 */
static void up_multi_click_handler(ClickRecognizerRef recognizer, void *context)
{
    uint8_t click_count = click_number_of_clicks_counted(recognizer);
    step_time_by_clicks(click_count, 1);
}

/**
 * DOWN button multi-click handler
 * 1 click = 1 day, 2 clicks = 7 days, 3 clicks = 30 days, 4+ clicks = 365 days
 */
static void down_multi_click_handler(ClickRecognizerRef recognizer, void *context)
{
    uint8_t click_count = click_number_of_clicks_counted(recognizer);
    step_time_by_clicks(click_count, -1);
}

/**
 * SELECT button handler - reset to current time
 */
static void select_click_handler(ClickRecognizerRef recognizer, void *context)
{
    time_step_days = 1;
    simulation_time = time(NULL);
    struct tm *time_info = localtime(&simulation_time);
    update_planet_positions(time_info);
    update_date_display();
}

/**
 * UP long click handler - start continuous forward stepping
 */
static void up_long_click_handler(ClickRecognizerRef recognizer, void *context)
{
    step_direction = 1;
    // Cancel existing timer
    if (step_timer)
        app_timer_cancel(step_timer);

    // Start stepping
    step_timer = app_timer_register(NEXT_STEP_DELAY, step_timer_callback, NULL);
}

/**
 * DOWN long click handler - start continuous backward stepping
 */
static void down_long_click_handler(ClickRecognizerRef recognizer, void *context)
{
    step_direction = -1;

    // Cancel existing timer
    if (step_timer)
        app_timer_cancel(step_timer);

    // Start stepping
    step_timer = app_timer_register(NEXT_STEP_DELAY, step_timer_callback, NULL);
}

/**
 * Button release handler - stop continuous stepping
 */
static void button_release_handler(ClickRecognizerRef recognizer, void *context)
{
    step_direction = 0;
    if (step_timer)
    {
        app_timer_cancel(step_timer);
        step_timer = NULL;
    }
}

/**
 * Click config provider
 */
static void click_config_provider(void *context)
{
    // Support up to 4 clicks - last_click_only=false means handler called after each click
    window_multi_click_subscribe(BUTTON_ID_UP, 1, 4, 0, false, up_multi_click_handler);
    window_multi_click_subscribe(BUTTON_ID_DOWN, 1, 4, 0, false, down_multi_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);

    // Long press handlers for continuous stepping
    window_long_click_subscribe(BUTTON_ID_UP, LONG_PRESS_DELAY, up_long_click_handler, button_release_handler);
    window_long_click_subscribe(BUTTON_ID_DOWN, LONG_PRESS_DELAY, down_long_click_handler, button_release_handler);
}

/**
 * Main window load handler
 * @param window The window being loaded
 */
static void main_window_load(Window *window)
{
    window_set_background_color(window, GColorBlack);
    GRect bounds = window_get_bounds(window);

    // Create background layer for planets
    background = layer_create(bounds);
    init_solar_system();
    load_solar_system(background);
    layer_add_to_window(background, window);

    // Create date text layer at top of screen
    date_layer = text_layer_create(GRect(0, 5, bounds.size.w, 30));
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_text_color(date_layer, GColorWhite);
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));

    // Initialize simulation time to current time
    simulation_time = time(NULL);
    update_date_display();

    // Set up button handlers
    window_set_click_config_provider(window, click_config_provider);
}

/**
 * Main window unload handler
 * @param window The window being unloaded
 */
static void main_window_unload(Window *window)
{
    text_layer_destroy(date_layer);
    layer_destroy(background);
}

/**
 * Initialize the app
 */
static void init()
{
    main_window = window_create();
    window_handlers(main_window, main_window_load, main_window_unload);
    window_stack_push(main_window, true);
}

/**
 * Deinitialize the app
 */
static void deinit()
{
    window_destroy_safe(main_window);
}

/**
 * Main entry point
 */
int main(void)
{
    init();
    app_event_loop();
    deinit();
}