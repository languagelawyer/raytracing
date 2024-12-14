#pragma once

#include "ray.hpp"
#include "types.hpp"

#include <cstddef>
#include <functional>


namespace rt
{
	/* 
	 * Left coordinate system:
	 * - X is to the right, Y is going upwards, Z -- forwards
	 */
	struct camera
	{
		camera(std::size_t width, std::size_t height);

		ray operator()(std::size_t x, std::size_t y) const
		{
			auto pixel = pixel00 + flt(x) * du + flt(y) * dv;
			auto direction = pixel - camera_pos;

			return { camera_pos, direction };
		}

	private:
		point3 camera_pos = { 0, 0, 0 };
		vec3 du;
		vec3 dv;
		point3 top_left;
		point3 pixel00;
	};
}
