#pragma once
#include "Definitions.h"
#include "GoGame.h"
#include "TextureManager.h"

namespace GameRenderer {
    sf::Vector2f toScreenCoord(int x, int y);
    void drawBoard(sf::RenderTarget& window, const GameSettings& settings);
    void drawStones(sf::RenderTarget& window, const GoGame& game, const StoneTextureManager& tm, const GameSettings& settings);
}