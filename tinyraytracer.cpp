#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
# define M_PI 3.14159265358979323846

#include "geometry.h"


struct Sphere
{
  Vec3f center;
  float radius;

  Sphere(const Vec3f &c, const float &r): center(c), radius(r) {}

  bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &t0) const
  {
    Vec3f L = center - orig;
    float tca = L * dir;
    float d2 = L * L - tca * tca;
    if (d2 > radius * radius) return false;
    float thc = sqrtf(radius * radius - d2);
    t0 = tca - thc;
    float t1 = tca + thc;
    if (t0 < 0) t0 = t1;
    if (t0 < 0) return false;
    return true;
  }
};

Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const Sphere &sphere)
{
  float sphere_dist = std::numeric_limits<float>::max();

  if (!sphere.ray_intersect(orig, dir, sphere_dist))
    {
      return Vec3f(0.969, 0.906, 0.863);
    }

  return Vec3f(0.251, 0.365, 0.447);
}

void render(const Sphere &sphere)
{
  const int width = 1024;
  const int height = 768;
  const int fov = M_PI / 2.;
  std::vector<Vec3f> framebuffer(width * height);

  #pragma omp parallel for
  for(size_t j = 0; j < height; j++)
    {
      for(size_t i = 0; i < width; i++)
	{
	  float x = (2 * (i + 0.5) / (float)width - 1) * tan(fov / 2.) * width / (float)height;
	  float y = -(2 * (j + 0.5) / (float)height - 1) * tan(fov / 2.);
	  Vec3f dir = Vec3f(x, y, -1).normalize();
	  framebuffer[i + j * width] = cast_ray(Vec3f(0, 0, 0), dir, sphere);
	}
    }

  std::ofstream ofs;
  ofs.open("./out.ppm", std::ofstream::out | std::ofstream::binary);
  ofs << "P6\n" << width << " " << height << "\n255\n";
  for(size_t i = 0; i < height * width; ++i)
    {
      for(size_t j = 0; j < 3; j++)
	{
	  ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
	}
    }
  ofs.close();
}

int main()
{
  Sphere sphere(Vec3f(-3, 0, -16), 2);
  render(sphere);
}
