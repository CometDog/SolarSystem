#include "base.h"

typedef enum
{
    MERCURY,
    VENUS,
    EARTH,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE
} PLANET;

void update_planet_positions();
void update_planet_position(PLANET planet, int angle);
void load_solar_system(Layer *layer);
void unload_solar_system(Layer *layer);
void init_solar_system();