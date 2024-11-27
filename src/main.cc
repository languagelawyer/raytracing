#include <SFML/Graphics.hpp>

#include <stdexcept>


sf::Image paint(unsigned width, unsigned height)
{
	sf::Image ret;
	ret.create(width, height);

	for (unsigned y = 0; y < height; y++)
	{
		for (unsigned x = 0; x < width; x++)
		{
			sf::Uint8 r = 255.999 * x / (width - 1);
			sf::Uint8 g = 255.999 * y / (height - 1);
			sf::Uint8 b = 0;

			ret.setPixel(x, y, {r, g, b});
		}
	}

	return ret;
}

int main()
{
	constexpr int window_w = 256;
	constexpr int window_h = 256;

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

			sf::Texture texture;
			if (!texture.loadFromImage(paint(width, height)))
				throw std::runtime_error("texture.loadFromImage");
			sf::Sprite sprite(texture);

			window.draw(sprite);
			window.display();

			need_redraw = false;
		}
	}
}
