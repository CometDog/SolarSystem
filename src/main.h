#include "planets.h"

// Time limits to prevent overflow
// Max: Jan 19, 2038, Min: Jan 1, 1970 (Unix epoch)
#define MAX_TIME_T 2147483647L
#define MIN_TIME_T 0

static Window *main_window;
static Layer *background;
static TextLayer *date_layer;

static AppTimer *timer = NULL;
static AppTimer *step_timer = NULL;

/**
 * Whether the watch has not been interacted with for a while
 */
static bool idle = true;

static time_t simulation_time = 0;
static int time_step_days = 1;
static int step_direction = 0; // 1 for forward, -1 for backward, 0 for stopped
