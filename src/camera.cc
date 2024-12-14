#include "camera.hpp"

#include <glm/geometric.hpp>

#include <algorithm>

using namespace rt;


camera::camera(std::size_t width, std::size_t height)
{
	auto focal = 1.;

	auto min = std::min(width, height);
	auto w = 2.f * width / min;
	auto h = 2.f * height / min;

	auto u = vec3(w, 0, 0);
	auto v = vec3(0, -h, 0);
	du = u / flt(width);
	dv = v / flt(height);

	top_left = camera_pos + vec3(0, 0, focal) - (u + v) / 2_flt;
	pixel00 = top_left + (du + dv) / 2_flt;
}
