#ifndef GOUIMANAGER_H
#define GOUIMANAGER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "GoGame.h"
#include "Definitions.h"
#include "Theme.h"

class GoUIManager {
private:
    GoGame& game;
    sf::Font font;

    // UI Elements
    sf::Text turnIndicator;
    sf::Text notificationText;

    std::unique_ptr<Theme::Button> passButton;

    // Store button + its logic label string
    std::vector<std::pair<std::unique_ptr<Theme::Button>, std::string>> m_buttons;

    void initializeIndicators();
    void initializeButtons();

public:
    GoUIManager(GoGame& g);
    void draw(sf::RenderWindow& window);

    bool handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window, GameState& currentGameState);

    void setNotification(const std::string& msg);
};

#endif