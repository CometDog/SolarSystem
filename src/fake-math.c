#include "fake-math.h"

/**
 * Sine lookup table (0-90 degrees inclusive)
 */
const int16_t sin_lookup_table[91] = {
    0, 18, 36, 54, 71, 89, 107, 125, 143, 160,
    178, 195, 213, 230, 248, 265, 282, 299, 316, 333,
    350, 367, 384, 400, 416, 433, 449, 465, 481, 496,
    512, 527, 543, 558, 573, 587, 602, 616, 630, 644,
    658, 672, 685, 698, 711, 724, 737, 749, 761, 773,
    784, 796, 807, 818, 829, 839, 850, 860, 869, 879,
    888, 897, 906, 914, 923, 931, 939, 946, 954, 961,
    968, 974, 980, 986, 992, 997, 1002, 1007, 1012, 1016,
    1020, 1024, 1027, 1030, 1033, 1035, 1037, 1039, 1040, 1042,
    1043};

/**
 * Get sine value for any angle (0-359)
 * @param angle The angle to use to look up the sine value
 */
int16_t fake_sin(int angle)
{
    // Correct angles out of range
    angle = angle % 360;
    if (angle < 0)
        angle += 360;

    // Lookup by quadrant
    if (angle <= 90)
    {
        return sin_lookup_table[angle]; // Right
    }
    else if (angle <= 180)
    {
        return sin_lookup_table[180 - angle]; // Bottom
    }
    else if (angle <= 270)
    {
        return -sin_lookup_table[angle - 180]; // Left
    }
    else
    {
        return -sin_lookup_table[360 - angle]; // Top
    }
}

/**
 * Get cosine value for any angle (0-359)
 * @param angle The angle to use to look up the cosine value
 */
int16_t fake_cos(int angle)
{
    // Cosine is sine shifted by 90 degrees
    return fake_sin(angle + 90);
}

/**
 * Determine floating point remainder for a division of x/y
 * @param x Dividend
 * @param y Divisor
 */
double fake_fmod(double x, double y)
{
    double quotient = x / y;
    double integer_part = (double)((long)quotient);
    if (quotient < 0 && quotient != integer_part)
    {
        integer_part -= 1.0;
    }
    return x - integer_part * y;
}