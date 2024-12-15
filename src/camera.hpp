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

		/**
		 * \brief Cast a ray from the camera through a given pixel (x, y), with some jitter
		 *
		 * \param fn a function that takes no arguments and returns vec2 for x and y random jitter.
		 *            The function shall return the offset relative to the pixel center.
		 *            The pixel is of relative size 1x1,
		 *            so  vec2(+0.5, +0.5) corresponds to the top right corner
		 *            and vec2(-0.5, -0.5) corresponds to the bottom left corner.
		 *            The values do not have to be in the [-0.5, +0.5] range,
		 *            e.g. a (truncated) Gaussian filter can be applied.
		 *            A Pixel Is *Not* A Little Square, after all.
		 * \param x the x coordinate of the pixel
		 * \param y the y coordinate of the pixel
		 */
		template<typename Fn>
		ray operator()(Fn fn, std::size_t x, std::size_t y) const
		{
			auto dxdy = fn();
			auto pixel = pixel00 + (x + dxdy.x) * du + (y + dxdy.y) * dv;
			auto direction = pixel - camera_pos;

			return { camera_pos, direction };
		}

		ray operator()(std::size_t x, std::size_t y) const
		{
			return operator()([] { return vec2(0, 0); }, x, y);
		}

	private:
		point3 camera_pos = { 0, 0, 0 };
		vec3 du;
		vec3 dv;
		point3 top_left;
		point3 pixel00;
	};
}
