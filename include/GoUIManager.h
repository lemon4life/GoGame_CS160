#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Definitions.h"
#include "GoGame.h" // Needs full definition of GoGame

class GoUIManager {
private:
    sf::Font font;
    GoGame& game;

    sf::Text turnIndicator;
    sf::Text notificationText;

    std::vector<sf::RectangleShape> mainButtonRects;
    std::vector<sf::Text> mainButtonTexts;
    std::vector<std::string> buttonLabels;

    void initializeIndicators();
    void initializeButtons();

public:
    explicit GoUIManager(GoGame& g);

    // Set a message on the UI (e.g. "Stone Placed")
    void setNotification(const std::string& msg);

    // Returns true if a button was clicked
    bool handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window, GameState& currentGameState);

    void draw(sf::RenderTarget& window);
};