#include "planets.h"
#include "fake-math.h"

#define CENTER_X 72
#define CENTER_Y 72

/**
 * Represents a planet layer with coordinates on the screen
 */
typedef struct
{
    int fake_orbit;
    int x;
    int y;
    int size;
    double period_days;
    int position_epoch;
    double eccentricity;
    int perihelion;
} PlanetLayer;

/**
 * Represents solar system layer containing all planets
 */
typedef struct
{
    PlanetLayer *sun;
    PlanetLayer *mercury;
    PlanetLayer *venus;
    PlanetLayer *earth;
    PlanetLayer *mars;
    PlanetLayer *jupiter;
    PlanetLayer *saturn;
    PlanetLayer *uranus;
    PlanetLayer *neptune;
} SolarSystemLayer;

/**
 * Reference to Solar System layer containing all PlanetLayers
 */
SolarSystemLayer *solar_system = NULL;

/**
 * Function to return the PlanetLayer from the given planet enum value
 * @param planet The PLANET enum value specifying which PlanetLayer to return
 */
PlanetLayer *get_planet_layer_from_planet(PLANET planet)
{
    PlanetLayer *planet_layer = NULL;
    switch (planet)
    {
    case MERCURY:
        planet_layer = solar_system->mercury;
        break;
    case VENUS:
        planet_layer = solar_system->venus;
        break;
    case EARTH:
        planet_layer = solar_system->earth;
        break;
    case MARS:
        planet_layer = solar_system->mars;
        break;
    case JUPITER:
        planet_layer = solar_system->jupiter;
        break;
    case SATURN:
        planet_layer = solar_system->saturn;
        break;
    case URANUS:
        planet_layer = solar_system->uranus;
        break;
    case NEPTUNE:
        planet_layer = solar_system->neptune;
        break;
    }

    return planet_layer;
}

/**
 * Calculate days since epoch (March 18, 2025) for a given date
 * @param year Year of requested time since date
 * @param month Month of requested time since date (January is 1)
 * @param day Day of requested time since date (Day in a month, not year)
 */
double days_since_epoch(int year, int month, int day)
{
    // Get supplied time
    struct tm time_info = {0};
    time_info.tm_year = year - 1900;
    time_info.tm_mon = month - 1;
    time_info.tm_mday = day;
    time_info.tm_hour = 12;

    time_t target_time = mktime(&time_info);

    // Epoch (March 18, 2025)
    struct tm epoch_tm = {0};
    epoch_tm.tm_year = 125; // 2025 - 1900
    epoch_tm.tm_mon = 2;    // March (mnoths are 0 indexed)
    epoch_tm.tm_mday = 18;
    epoch_tm.tm_hour = 0;

    time_t epoch_time = mktime(&epoch_tm);

    // Calculate difference in days
    return difftime(target_time, epoch_time) / (60 * 60 * 24);
}

/**
 * Calculate angular position of a planet at given days from epoch
 * @param planet Enum value of planet to determine angle for
 * @param days Days since the epoch to determine current angle from
 */
int calculate_planet_angle(PLANET planet, double days)
{
    PlanetLayer *planet_layer = get_planet_layer_from_planet(planet);
    if (!planet_layer)
        return -1;

    // Calculating formula:  θ ≈ R + 2e*sin(M) (derived from Kepler's Equation: θ ≈ M + 2e*sin(M))
    // R: Reference frame. We adjust the reference frame position which is on the watch face, rather than calculate the anomaly to the perihelion as the original equation would do
    // Calculate position if orbit were circular
    double circular_position = planet_layer->position_epoch - (days * 360.0 / planet_layer->period_days);
    circular_position = fake_fmod(circular_position + 360.0, 360.0);

    // M
    // Calculate angular distance from perihelion
    double mean_anomaly = fake_fmod(circular_position - planet_layer->perihelion + 360.0, 360.0);

    // 2e * sin(M)
    // Simulate faster motion near the perihleion and slower motion near the anthelion to simulate an elliptical orbit
    double elliptical_correction = 2.0 * planet_layer->eccentricity * fake_sin(mean_anomaly * PI / 180.0);

    // R + 2e*sin(M)
    // Calculate elliptical position on the circular plane
    double actual_position = circular_position + elliptical_correction;

    // Adjust to 0-360 degrees
    return (int)fake_fmod(actual_position + 360.0, 360.0);
}

/**
 * Function to update planet positions based on angle for a given PlanetLayer
 * @param planet_layer The PlanetLayer to update
 * @param angle Angle at which the planet should sit on its orbital circle
 */
void update_planet_layer_position(PlanetLayer *planet_layer, int angle)
{
    float scale = 1.0f / 1024.0f;

    planet_layer->x = CENTER_X + (int)(planet_layer->fake_orbit * fake_cos(angle) * scale);
    planet_layer->y = CENTER_Y + (int)(planet_layer->fake_orbit * fake_sin(angle) * scale);
}

/**
 * Function to update planet positions based on angle for a given PLANET enum value
 * @param planet PLANET enum value representing the PlanetLayer to update
 * @param angle Angle at which the planet should sit on its orbital circle
 */
void update_planet_position(PLANET planet, int angle)
{
    PlanetLayer *planet_layer = get_planet_layer_from_planet(planet);
    if (!planet_layer)
        return;

    update_planet_layer_position(planet_layer, angle);
}

/**
 * Update the positions of all planets in the solar system based on current day
 */
void update_planet_positions()
{
    time_t epoch = time(NULL);
    struct tm *t = localtime(&epoch);
    double days = days_since_epoch(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    update_planet_position(MERCURY, calculate_planet_angle(MERCURY, days));
    update_planet_position(VENUS, calculate_planet_angle(VENUS, days));
    update_planet_position(EARTH, calculate_planet_angle(EARTH, days));
    update_planet_position(MARS, calculate_planet_angle(MARS, days));
    update_planet_position(JUPITER, calculate_planet_angle(JUPITER, days));
    update_planet_position(SATURN, calculate_planet_angle(SATURN, days));
    update_planet_position(URANUS, calculate_planet_angle(URANUS, days));
    update_planet_position(NEPTUNE, calculate_planet_angle(NEPTUNE, days));
}

/**
 * Update all planets in the solar system with their correct position
 * @param layer Solar system layer to update. Unused
 * @param context Graphics context to use during update. Unused
 */
void layer_update_solar_system(Layer *layer, GContext *context)
{
    graphics_context_set_fill_color(context, GColorYellow);
    graphics_fill_circle(context, GPoint(solar_system->sun->x, solar_system->sun->y), solar_system->sun->size);

    graphics_context_set_fill_color(context, GColorLightGray);
    graphics_fill_circle(context, GPoint(solar_system->mercury->x, solar_system->mercury->y), solar_system->mercury->size);

    graphics_context_set_fill_color(context, GColorBrass);
    graphics_fill_circle(context, GPoint(solar_system->venus->x, solar_system->venus->y), solar_system->venus->size);

    graphics_context_set_fill_color(context, GColorBlueMoon);
    graphics_fill_circle(context, GPoint(solar_system->earth->x, solar_system->earth->y), solar_system->earth->size);

    graphics_context_set_fill_color(context, GColorRed);
    graphics_fill_circle(context, GPoint(solar_system->mars->x, solar_system->mars->y), solar_system->mars->size);

    graphics_context_set_fill_color(context, GColorRajah);
    graphics_fill_circle(context, GPoint(solar_system->jupiter->x, solar_system->jupiter->y), solar_system->jupiter->size);

    graphics_context_set_fill_color(context, GColorChromeYellow);
    graphics_fill_circle(context, GPoint(solar_system->saturn->x, solar_system->saturn->y), solar_system->saturn->size);

    graphics_context_set_fill_color(context, GColorCeleste);
    graphics_fill_circle(context, GPoint(solar_system->uranus->x, solar_system->uranus->y), solar_system->uranus->size);

    graphics_context_set_fill_color(context, GColorVividCerulean);
    graphics_fill_circle(context, GPoint(solar_system->neptune->x, solar_system->neptune->y), solar_system->neptune->size);

    update_planet_positions();
}

/**
 * Load the solar system onto the given layer
 * @param layer The layer onto which the solar system will be loaded
 */
void load_solar_system(Layer *layer)
{
    // Sun at center
    PlanetLayer *sun = malloc(sizeof(PlanetLayer));
    sun->fake_orbit = 0;
    sun->size = 8;
    sun->x = CENTER_X;
    sun->y = CENTER_Y;
    sun->period_days = 0.0;
    sun->position_epoch = 0;
    sun->eccentricity = 0.0;
    sun->perihelion = 0;
    solar_system->sun = sun;

    // Mercury
    PlanetLayer *mercury = malloc(sizeof(PlanetLayer));
    mercury->fake_orbit = 13;
    mercury->size = 1;
    mercury->x = CENTER_X;
    mercury->y = CENTER_Y + mercury->fake_orbit;
    mercury->period_days = 87.97;
    mercury->position_epoch = 180;
    mercury->eccentricity = 0.2056;
    mercury->perihelion = 226;
    solar_system->mercury = mercury;

    // Venus
    PlanetLayer *venus = malloc(sizeof(PlanetLayer));
    venus->fake_orbit = 19;
    venus->size = 1;
    venus->x = CENTER_X;
    venus->y = CENTER_Y + venus->fake_orbit;
    venus->period_days = 224.70;
    venus->position_epoch = 180;
    venus->eccentricity = 0.0068;
    venus->perihelion = 280;
    solar_system->venus = venus;

    // Earth
    PlanetLayer *earth = malloc(sizeof(PlanetLayer));
    earth->fake_orbit = 25;
    earth->size = 1;
    earth->x = CENTER_X;
    earth->y = CENTER_Y + earth->fake_orbit;
    earth->period_days = 365.26;
    earth->position_epoch = 180;
    earth->eccentricity = 0.0167;
    earth->perihelion = 252;
    solar_system->earth = earth;

    // Mars
    PlanetLayer *mars = malloc(sizeof(PlanetLayer));
    mars->fake_orbit = 31;
    mars->size = 1;
    mars->x = CENTER_X;
    mars->y = CENTER_Y + mars->fake_orbit;
    mars->period_days = 686.98;
    mars->position_epoch = 215;
    mars->eccentricity = 0.0934;
    mars->perihelion = 485;
    solar_system->mars = mars;

    // Jupiter
    PlanetLayer *jupiter = malloc(sizeof(PlanetLayer));
    jupiter->fake_orbit = 41;
    jupiter->size = 5;
    jupiter->x = CENTER_X;
    jupiter->y = CENTER_Y + jupiter->fake_orbit;
    jupiter->period_days = 4332.59;
    jupiter->position_epoch = 260;
    jupiter->eccentricity = 0.484;
    jupiter->perihelion = 163;
    solar_system->jupiter = jupiter;

    // Saturn
    PlanetLayer *saturn = malloc(sizeof(PlanetLayer));
    saturn->fake_orbit = 52;
    saturn->size = 4;
    saturn->x = CENTER_X;
    saturn->y = CENTER_Y + saturn->fake_orbit;
    saturn->period_days = 10759.22;
    saturn->position_epoch = 5;
    saturn->eccentricity = 0.0542;
    saturn->perihelion = 241;
    solar_system->saturn = saturn;

    // Uranus
    PlanetLayer *uranus = malloc(sizeof(PlanetLayer));
    uranus->fake_orbit = 61;
    uranus->size = 2;
    uranus->x = CENTER_X;
    uranus->y = CENTER_Y + uranus->fake_orbit;
    uranus->period_days = 30688.50;
    uranus->position_epoch = 300;
    uranus->eccentricity = 0.0472;
    uranus->perihelion = 319;
    solar_system->uranus = uranus;

    // Neptune
    PlanetLayer *neptune = malloc(sizeof(PlanetLayer));
    neptune->fake_orbit = 68;
    neptune->size = 2;
    neptune->x = CENTER_X;
    neptune->y = CENTER_Y + neptune->fake_orbit;
    neptune->period_days = 60195.00;
    neptune->position_epoch = 355;
    neptune->eccentricity = 0.0086;
    neptune->perihelion = 193;
    solar_system->neptune = neptune;

    layer_set_update_proc(layer, layer_update_solar_system);
}

/**
 * Unload solar system, tearing down all UI that has been placed onto the given layer
 * @param layer The layer on which the solar system to unload exists
 */
void unload_solar_system(Layer *layer)
{
}

/**
 * Initialize the solar system layer
 */
void init_solar_system()
{
    solar_system = malloc(sizeof(SolarSystemLayer));
}