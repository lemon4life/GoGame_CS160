#pragma once // Prevents including this file multiple times

#include <SFML/Graphics.hpp>

class Game {
public:
    Game();
    void run();

private:
    sf::RenderWindow mWindow;
    sf::CircleShape mPlayer; // Just a test shape
};H