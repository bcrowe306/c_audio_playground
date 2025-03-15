#include "utility.h"

double logarithmicMap(double value, double minValue, double maxValue,
                      double minFreq, double maxFreq) {
  double minLog = std::log10(minFreq);
  double maxLog = std::log10(maxFreq);
  double valueRange = maxValue - minValue;
  double logFreq = minLog + (value / valueRange) * (maxLog - minLog);
  return std::pow(10.0, logFreq);
}

// Single precision (float) version
float logarithmicMap_float(float value, float minValue, float maxValue,
                           float minFreq, float maxFreq) {
  float minLog = std::log10(minFreq);
  float maxLog = std::log10(maxFreq);
  float valueRange = maxValue - minValue;
  float logFreq = minLog + (value / valueRange) * (maxLog - minLog);
  return std::pow(10.0f, logFreq);
}


// Convert db to linear
double db_to_linear(double db)
{
    return std::pow(10, db / 20.0);
}

// Convert linear to db
float linear_to_db(float linear)
{
    return 20 * std::log10(linear);
}

// Lineear interpolation function for integers
int lerp(int a, int b, float t)
{
    return a + t * (b - a);
}

// Linear interpolation function for floats
float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

// Linear interpolation function for doubles
double lerp(double a, double b, double t)
{
    return a + t * (b - a);
}

// Linear interpolation function for long doubles

long double lerp(long double a, long double b, long double t)
{
    return a + t * (b - a);
}

// Linear interpolation function for unsigned integers
unsigned int lerp(unsigned int a, unsigned int b, float t)
{
    return a + t * (b - a);
}

// Map function for integers
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Map function for floats
float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Map function for floats
float map_float(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float weighted_map_float(float value, float value_min, float value_max, float scale_min, float scale_max, float mid_point, bool logarithmic){
    float value_midpoint = (value_max - value_min) / 2 + value_min;

    if (value < value_midpoint){
        if (logarithmic){
            return 0.0 - map_float(value, value_min, value_midpoint, scale_min, mid_point);
        } else {
            return 0.0 - map_float(value, value_min, value_midpoint, scale_min, mid_point);
        }
        return 0.0 - map_float(value, value_min, value_midpoint, scale_min, mid_point);
    } else {
        if (logarithmic){
          return map_float(value, value_midpoint, value_max, mid_point,
                           scale_max);
        } else {
            return map_float(value, value_midpoint, value_max, mid_point, scale_max);
        }
    }
};

// float constrain(float x, float min_val, float max_val)
// {
//     if (x < min_val)
//         return min_val;
//     if (x > max_val)
//         return max_val;
//     return x;
// }

// // int constrain()

// int constrain(int x, int min_val, int max_val)
// {
//     if (x < min_val)
//         return min_val;
//     if (x > max_val)
//         return max_val;
//     return x;
// }

// constrain()
//
// Clips an input value to be between two end points
// x is the value to constrain; min_val and max_val are the range


//
// max()
float max(float x, float y)
{
    return x > y ? x : y;
}

// min()
float min(float x, float y)
{
    return x < y ? x : y;
}

float scale_midi_to_unit(float value, float sensitivity, float range, float shift)
{
    return ((value / 127 - 0.5) * range) * sensitivity + shift;
}

// Double precision version
