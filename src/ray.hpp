#pragma once

#include "types.hpp"

#include <glm/geometric.hpp>

namespace rt
{
	class ray
	{
		point3 o;
		vec3 dir;

	public:
		ray(point3 origin, const vec3& direction)
		: o(std::move(origin))
		, dir(glm::normalize(direction))
		{}

		const point3& origin() const { return o; }
		const vec3& direction() const { return dir; }

		auto at(flt t) const
		{ return o + t * dir; }
	};
}
