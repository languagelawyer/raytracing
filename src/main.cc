#include <SFML/Graphics.hpp>


int main()
{
	constexpr int image_width = 256;
	constexpr int image_height = 256;

	sf::Image image;
	image.create(image_width, image_height);

	for (int y = 0; y < image_height; y++)
	{
		for (int x = 0; x < image_width; x++)
		{
			sf::Uint8 r = 255.999 * x / (image_width - 1);
			sf::Uint8 g = 255.999 * y / (image_height - 1);
			sf::Uint8 b = 0;

			image.setPixel(x, y, {r, g, b});
		}
	}

	sf::Texture texture;
	if (!texture.loadFromImage(image))
		return -1;

	sf::Sprite sprite(texture);

	sf::RenderWindow window({image_width, image_height}, "");
	while (window.isOpen())
	{
		window.clear();
		window.draw(sprite);
		window.display();

		sf::Event event;
		while (window.waitEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
	}
}
