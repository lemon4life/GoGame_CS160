#pragma once
#include <SFML/Graphics.hpp>
#include "Definitions.h"
#include "GoGame.h"
#include "TextureManager.h"

namespace GameRenderer {
    sf::Vector2f toScreenCoord(int x, int y);

    void drawBoard(sf::RenderWindow& window, const GameSettings& settings, const sf::Font& font);

    void drawStones(sf::RenderWindow& window, const GoGame& game, const StoneTextureManager& tm, const GameSettings& settings);

    void invalidateCache();
}