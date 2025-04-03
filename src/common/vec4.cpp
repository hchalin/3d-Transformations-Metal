#include "vec4.h"

vec4::vec4(float x, float y, float z, float w) : v{x, y, z, w}{}

float vec4::x()const { return v[0]; }
float vec4::y()const { return v[1]; }
float vec4::z()const { return v[2]; }
float vec4::w()const { return v[3]; }
