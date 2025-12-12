#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Theme.h" // <--- Include the new class

class NewGameScreen {
private:
    sf::RectangleShape background;
    sf::RectangleShape container;
    sf::Text titleText;

    std::vector<std::pair<Theme::Button, std::string>> m_buttons;

public:
    void init(const sf::Font& font, float width, float height);
    void draw(sf::RenderWindow& window);
    std::string handleMouseClick(const sf::Vector2i& mousePos);
};