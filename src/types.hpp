#pragma once

#include <glm/ext/vector_float3.hpp>

namespace rt
{
	using vec3 = glm::vec3;
	using flt = vec3::value_type;
	inline flt operator ""_flt(long double v) { return flt(v); }
	inline flt operator ""_flt(unsigned long long v) { return flt(v); }

	using point3 = vec3;
	using color = vec3;
}
