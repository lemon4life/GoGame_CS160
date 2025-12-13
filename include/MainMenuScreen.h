#ifndef MAINMENUSCREEN_H
#define MAINMENUSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "GoGame.h"
#include "Definitions.h"
#include "Theme.h" // Includes Theme::Button

class MainMenuScreen {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    sf::Font font;
    sf::Font titleFont;
    sf::Text titleText;

    // Optimized: Store buttons in a list
    std::vector<std::pair<Theme::Button, std::string>> m_buttons;

public:
    MainMenuScreen();
    void draw(sf::RenderWindow& window);

    // Returns the next GameState
    GameState handleEvent(const sf::Event& event, const sf::Vector2i& mousePos, GoGame& game);
};

#endif