#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Definitions.h"

class NewGameScreen {
private:
    sf::RectangleShape background;
    sf::RectangleShape container;
    sf::Text titleText;

    struct OptionButton {
        sf::RectangleShape rect;
        sf::Text text;
        std::string action; // "PVP", "AI_EASY", "AI_MEDIUM", "AI_HARD", "BACK"
    };
    std::vector<OptionButton> buttons;

public:
    void init(const sf::Font& font, float width, float height);
    void draw(sf::RenderWindow& window);
    std::string handleMouseClick(const sf::Vector2i& mousePos);
};