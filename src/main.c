#include "main.h"

/**
 * Timer callback to set the idle flag
 * @param data Unused
 */
static void timer_callback(void *data)
{
  idle = true;
}

/**
 * Register 3 minute timer to set idle status
 */
static void register_idle_timer()
{
  idle = false;
  app_timer_cancel(timer);
  timer = app_timer_register(180 * 1000, timer_callback, NULL);
}

/**
 * Updates the time and triggers animations
 */
static void update_time()
{
  update_planet_positions();
}

/**
 * Tap handler to reset the idle timer
 * @param axis The axis of the tap. Unused
 * @param direction The direction of the tap. Unused
 */
static void tap_handler(AccelAxisType axis, int32_t direction)
{
  register_idle_timer();
}

/**
 * Bluetooth connection handler to vibrate on connection status change
 * @param connected Whether the connection is established
 */
static void bt_handler(bool connected)
{
  if (connected)
  {
    vibes_short_pulse();
  }
  else
  {
    vibes_double_pulse();
  }
}

/**
 * Handle time tick event
 * @param tick_time Pointer to the time structure. Unused
 * @param units_changed The units that have changed.
 */
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  // Update time every minute
  if (units_changed & MINUTE_UNIT)
  {
    update_time();
  }
}

/**
 * Main window load handler
 * @param window The window being loaded
 */
static void main_window_load(Window *window)
{
  window_set_background_color(window, GColorBlack);
  GRect bounds = window_get_bounds(window);
  background = layer_create(bounds);
  init_solar_system();
  load_solar_system(background);
  layer_add_to_window(background, window);
}

/**
 * Main window unload handler
 * @param window The window being unloaded
 */
static void main_window_unload(Window *window)
{
}

/**
 * Initialize the app
 */
static void init()
{
  main_window = window_create();
  window_handlers(main_window, main_window_load, main_window_unload);
  window_stack_push(main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
  bluetooth_connection_service_subscribe(bt_handler);

  register_idle_timer();
}

/**
 * Deinitialize the app
 */
static void deinit()
{
  animation_unschedule_all();
  accel_tap_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
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