#pragma once

#include "types.hpp"

namespace rt
{
	struct ray
	{
		const point3 origin;
		const vec3 direction;

		auto at(flt t) const
		{ return origin + t * direction; }
	};
}
