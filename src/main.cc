#include "camera.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "threadpool.hpp"
#include "types.hpp"

#include <SFML/Graphics.hpp>

#include <glm/geometric.hpp>
#include <glm/gtx/intersect.hpp>

#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace rt;

std::random_device rd;
thread_local std::mt19937 engine(rd());

class sky : public hittable
{
	bool hit(const ray& r, hit_record& rec) override
	{
		rec.t = std::numeric_limits<flt>::max();
		rec.next.reset();

		auto a = (r.direction().y + 1) / 2;
		rec.material = [=](color& c) {
			c = (1 - a) * color(1, 1, 1) + a * color(.5, .7, 1);
		};

		return true;
	}
};

class diffuse_sphere : public hittable
{
	const point3 center;
	const flt radius;

public:
	diffuse_sphere(point3 center, flt radius)
	: center(std::move(center))
	, radius(radius)
	{}

	bool hit(const ray& r, hit_record& rec) override
	{
		flt dst;
		auto hits = glm::intersectRaySphere(r.origin(), r.direction(), center, radius * radius, dst);

		if (!hits) return false;

		auto hitpoint = r.at(dst);
		auto normal = (hitpoint - center) / radius;

		// The following seems to fix shadow acne, but whatever,
		// lets use a more generic fix (see below)
		// Although, per-object fix taking into account its geometry
		// can also make sense
		// if (glm::dot(r.direction(), normal) >= 0) return false;

		rec.t = dst;
		rec.normal = normal;

		std::uniform_real_distribution<flt> distr(-1, 1);
		while (true) {
			auto p = vec3(distr(engine), distr(engine), distr(engine));
			auto len = glm::length(p);

			if (len >= 16 * std::numeric_limits<flt>::min() and len <= 1) {
				// could not do it with glm::faceforward
				if (glm::dot(p, normal) < 0) p = -p;
				// Lambertian reflection
				rec.next.emplace(hitpoint, normal + p / std::sqrt(len));
				break;
			}
		}

		rec.material = [](color& c) {
			c *= 0.5;
		};

		return true;
	}
};

template<std::size_t maxHits>
class world
{
	std::vector<std::shared_ptr<hittable>> objects = {
		std::make_shared<sky>(),
		std::make_shared<diffuse_sphere>(point3(0, 0, 1), 0.5),
		std::make_shared<diffuse_sphere>(point3(0, -100.5, 1), 100),
	};

public:
	color ray_color(const ray& r)
	{
		std::size_t i = 0;
		hit_record hits[maxHits];

		auto cur_ray = r;
		while (i < maxHits)
		{
			auto& closest_hit = hits[i++];
			hit_record current_hit = closest_hit = {
				.t = std::numeric_limits<flt>::infinity(),
				.material = [](color&) { /* does nothing */ },
			};

			for (const auto& o : objects)
			{
				if (!o->hit(cur_ray, current_hit)) continue;

				if (current_hit.t < closest_hit.t)
					closest_hit = current_hit;
			}

			if (!closest_hit.next) break;

			const auto& next_ray= *closest_hit.next;
			// moving away from the object is intended to fix the shadow acne
			cur_ray = { next_ray.origin() + closest_hit.normal * 0.00001_flt, next_ray.direction() };
		}

		color ret(0, 0, 0);
		while (i--) hits[i].material(ret);

		return ret;
	}
};

int main()
{
	std::vector<int> indices;
	std::vector<ray> rays;
	std::vector<color> sum;
	unsigned total = 0;

	std::vector<sf::Uint8> rgba;
	sf::Image image;

	world<16> w;
	auto fn  = [&](int idx) {
		auto i = indices[idx];
		sum[i] += w.ray_color(rays[i]);
	};
	threadpool pool(fn, std::thread::hardware_concurrency() ?: 2);

	unsigned width = 800;
	unsigned height = 600;
	sf::RenderWindow window({width, height}, "");
	while (window.isOpen())
	{
		sf::Event event;
		if (!window.pollEvent(event))
			goto no_events;

		if (event.type == sf::Event::Closed)
		{
			window.close();
			break;
		}

		if (event.type == sf::Event::Resized)
		{
			width = event.size.width;
			height = event.size.height;

			sf::FloatRect rect(0, 0, width, height);
			window.setView(sf::View(rect));

			indices.clear();
			rays.clear();
			camera cam(width, height);
			for (auto y = 0u; y < height; y++)
				for (auto x = 0u; x < width; x++)
					indices.push_back(rays.size()), rays.push_back(cam(x, y));

			sum.clear();
			sum.resize(width * height);
			total = 0;
			rgba.resize(4 * width * height);
		}

no_events:
		// Reshuffle from time to time, to avoid possible false sharing?
		// Or following the ray consumes most of the time,
		// so false-sharing at `sum[i] +=` is likely negligible?
		// if (total % 16 == 0) std::shuffle(indices.begin(), indices.end(), engine);

		pool.go(indices.size());
		total++;

		for (auto i = 0u; i < sum.size(); i++)
		{
			auto c = sum[i] / flt(total);

			for (int j = 0; j < 3; j++) rgba[4 * i + j] = 255.999 * c[j];
			rgba[4 * i + 3] = 255;
		}
		image.create(width, height, rgba.data());

		sf::Texture texture;
		if (!texture.loadFromImage(image))
			throw std::runtime_error("texture.loadFromImage");
		sf::Sprite sprite(texture);

		window.draw(sprite);
		window.display();
		window.setTitle(std::to_string(total));
	}
}
