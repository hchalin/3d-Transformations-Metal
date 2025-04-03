#pragma once

/*
  raytracinginoneweekend.com
*/

class vec4
{
public:
  vec4(float x, float y, float z, float w);
  ~vec4() = default; // No specialized destructor

  float x() const;
  float y() const;
  float z() const;
  float w() const;

private:
  float v[4];
};

using float4 = vec4;
