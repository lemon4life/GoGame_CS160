#ifndef TOGGLESCREEN_H
#define TOGGLESCREEN_H

#include <SFML/Graphics.hpp>
#include "GoGame.h"
#include "Definitions.h"

class ToggleScreen {
private:
    // UI Elements
    sf::RectangleShape sidebarCover; // Covers the old buttons
    sf::RectangleShape finishBtn;
    sf::Text finishBtnText;
    sf::Text instructionText;
    
    // Marker visual
    sf::RectangleShape deadMarker;

public:
    void init(const sf::Font& font, float width, float height);
    
    // Draws the UI and the dead markers overlay
    void draw(sf::RenderWindow& window, const GoGame& game);
    
    // Returns true if "Finish" is clicked
    bool handleClick(const sf::Vector2i& mousePos, GoGame& game);
};

#endif