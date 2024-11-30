#include "camera.hpp"
#include "types.hpp"

#include <glm/geometric.hpp>

#include <algorithm>

using namespace rt;

color& camera::buf::operator()(std::size_t x, std::size_t y) { return buf[y * width + x]; }

void camera::render(buf buf, query_f q)
{
	auto width = buf.width;
	auto height = buf.height;

	auto camera_pos = point3(0, 0, 0);
	auto focal = 1.;

	auto min = std::min(width, height);
	auto w = 2.f * width / min;
	auto h = 2.f * height / min;

	auto u = vec3(w, 0, 0);
	auto v = vec3(0, -h, 0);
	auto du = u / flt(width);
	auto dv = v / flt(height);

	auto top_left = camera_pos
	                    + vec3(0, 0, focal)
	                    - (u + v) / 2_flt;
	auto pixel00 = top_left + (du + dv) / 2_flt;

	for (unsigned y = 0; y < height; y++)
	{
		for (unsigned x = 0; x < width; x++)
		{
			auto pixel = pixel00 + flt(x) * du + flt(y) * dv;
			auto direction = pixel - camera_pos;

			buf(x, y) = q({ camera_pos, direction });
		}
	}
}