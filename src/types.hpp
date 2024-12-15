#pragma once

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

#include <type_traits>

namespace rt
{
	using vec2 = glm::vec2;
	using vec3 = glm::vec3;
	static_assert(std::is_same_v<vec2::value_type, vec3::value_type>);

	using flt = vec3::value_type;
	inline flt operator ""_flt(long double v) { return flt(v); }
	inline flt operator ""_flt(unsigned long long v) { return flt(v); }

	using point3 = vec3;
	using color = vec3;
}
