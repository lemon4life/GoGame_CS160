#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Definitions.h"
#include "GoGame.h"

class MainMenuScreen {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Font titleFont;
    sf::Font buttonFont;
    sf::Text titleText;

    std::vector<sf::RectangleShape> menuButtonRects;
    std::vector<sf::Text> menuButtonTexts;
    std::vector<std::string> buttonLabels;

    // Menu Layout Constants (Internal helper)
    const int MENU_BTN_WIDTH = 250;
    const int MENU_BTN_HEIGHT = 50;
    const int MENU_START_X = 100;
    const int MENU_START_Y = 250;

public:
    MainMenuScreen();

    void draw(sf::RenderWindow& window);
    
    // Returns the new state (e.g. PLAYING, SETTINGS) based on clicks
    GameState handleEvent(const sf::Event& event, const sf::Vector2i& mousePos, GoGame& game);
};