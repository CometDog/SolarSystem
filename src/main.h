#include "planets.h"

static Window *main_window;
static Layer *background;

static AppTimer *timer = NULL;

/**
 * Whether the watch has not been interacted with for a while
 */
static bool idle = true;
