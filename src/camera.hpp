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
		using query_f = std::function<color(const ray&)>;

		struct buf
		{
			const std::size_t width;
			const std::size_t height;

			color* const buf;

			color& operator()(std::size_t x, std::size_t y);
		};

		void render(buf buf, query_f query);
	};
}
