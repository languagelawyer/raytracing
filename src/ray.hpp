#pragma once

#include "types.hpp"

#include <glm/geometric.hpp>

namespace rt
{
	struct ray
	{
		const point3 origin;
		const vec3 direction;

		ray(point3 origin, const vec3& dir)
		: origin(std::move(origin))
		, direction(glm::normalize(dir))
		{}

		auto at(flt t) const
		{ return origin + t * direction; }
	};
}
