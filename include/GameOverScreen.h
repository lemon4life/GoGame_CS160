#ifndef GAMEOVERSCREEN_H
#define GAMEOVERSCREEN_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Definitions.h" 

class GameOverScreen {
private:
    sf::RectangleShape background;
    sf::RectangleShape container;
    sf::Text titleText;
    sf::Text scoreText;
    
    struct Button {
        sf::RectangleShape rect;
        sf::Text text;
        std::string action; // "NEW", "LOAD", "EXIT"
    };
    std::vector<Button> buttons;

public:
    void init(const sf::Font& font, float width, float height);
    void setGameOverMessage(const std::string& message, const std::string& scoreDetail);
    
    void draw(sf::RenderWindow& window);
    
    // Returns: "NEW", "LOAD", "EXIT", or "" (nothing)
    std::string handleMouseClick(const sf::Vector2i& mousePos);
};

#endif