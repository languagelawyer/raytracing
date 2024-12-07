#pragma once

#include "ray.hpp"
#include "types.hpp"

#include <functional>
#include <optional>


namespace rt
{
	struct hit_record
	{
		flt t;
		vec3 normal;

		std::optional<ray> next;

		// called with the incident ray color, can mutate it
		std::function<void(color&)> material;
	};

	struct hittable
	{
		virtual ~hittable() = default;

		virtual bool hit(const ray& r, hit_record& rec) = 0;
	};
}
