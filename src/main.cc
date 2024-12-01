#include "camera.hpp"
#include "ray.hpp"
#include "types.hpp"

#include <SFML/Graphics.hpp>

#include <glm/geometric.hpp>
#include <glm/gtx/intersect.hpp>

#include <stdexcept>
#include <vector>

using namespace rt;


color ray_color(const ray& r)
{
	const auto& dir = r.direction;

	vec3 pos, nrm;
	if (glm::intersectRaySphere(r.origin, dir, point3(0, 0, 1), 0.5, pos, nrm))
		return (nrm + 1_flt) / 2_flt;

	auto a = (dir.y + 1) / 2;

	return (1 - a) * color(1, 1, 1) + a * color(.5, .7, 1);
}

int main()
{
	const int window_w = 320;
	const int window_h = 240;

	std::vector<color> buf;
	std::vector<sf::Uint8> rgba;
	sf::Image image;

	sf::RenderWindow window({window_w, window_h}, "");
	while (window.isOpen())
	{
		bool need_redraw = false;

		sf::Event event;
		while (window.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::Resized) {
				auto [w, h] = event.size;
				sf::FloatRect rect(0, 0, w, h);

				window.setView(sf::View(rect));
				need_redraw = true;

				break;
			}
		}

		if (need_redraw)
		{
			auto [width, height] = window.getSize();

			buf.resize(width * height);
			camera{}.render({ width, height, buf.data() }, ray_color);

			rgba.resize(4 * width * height);
			for (unsigned i = 0; i < width * height; i++)
			{
				const auto& color = buf.data()[i];

				for (int j = 0; j < 3; j++) rgba[4 * i + j] = 255.999 * color[j];
				rgba[4 * i + 3] = 255;
			}

			image.create(width, height, rgba.data());

			sf::Texture texture;
			if (!texture.loadFromImage(image))
				throw std::runtime_error("texture.loadFromImage");
			sf::Sprite sprite(texture);

			window.draw(sprite);
			window.display();

			need_redraw = false;
		}
	}
}
