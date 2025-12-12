#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include "Definitions.h"
#include "Theme.h"

class GameOverScreen {
private:
    sf::RectangleShape background;
    sf::RectangleShape container;
    sf::Text titleText;
    sf::Text scoreText;

    // Store Buttons and their action strings
    std::vector<std::pair<Theme::Button, std::string>> m_buttons;

public:
    void init(const sf::Font& font, float width, float height);
    void setGameOverMessage(const std::string& message, const std::string& scoreDetail);

    void draw(sf::RenderWindow& window);

    // Returns: "NEW", "LOAD", "EXIT", or ""
    std::string handleMouseClick(const sf::Vector2i& mousePos);
};

#endif