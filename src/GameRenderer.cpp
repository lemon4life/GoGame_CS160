#include "GameRenderer.h"
#include <cmath>

sf::Vector2f GameRenderer::toScreenCoord(int x, int y) {
    return sf::Vector2f(GRID_OFFSET + x * CELL_SIZE, GRID_OFFSET + y * CELL_SIZE);
}

void GameRenderer::drawBoard(sf::RenderTarget& window, const GameSettings& settings) {
    sf::RectangleShape fullBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
    fullBackground.setFillColor(sf::Color(60, 60, 60));
    window.draw(fullBackground);

    sf::RectangleShape boardBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));

    // --- FIX 1: Use the color from Settings, not the hardcoded one ---
    boardBackground.setFillColor(settings.getBoardColor());

    window.draw(boardBackground);

    const sf::Color lineColor(0, 0, 0);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        sf::Vertex line1[] = {
            sf::Vertex(toScreenCoord(i, 0), lineColor),
            sf::Vertex(toScreenCoord(i, BOARD_SIZE - 1), lineColor)
        };
        window.draw(line1, 2, sf::PrimitiveType::Lines);

        sf::Vertex line2[] = {
            sf::Vertex(toScreenCoord(0, i), lineColor),
            sf::Vertex(toScreenCoord(BOARD_SIZE - 1, i), lineColor)
        };
        window.draw(line2, 2, sf::PrimitiveType::Lines);
    }
}

void GameRenderer::drawStones(sf::RenderTarget& window, const GoGame& game, const StoneTextureManager& tm, const GameSettings& settings) {
    sf::Sprite stoneSprite;

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            Stone s = game.getStoneAt(x, y);
            if (s == Stone::Empty) continue;

            // --- FIX 2: Use 'tm' (the variable) and pass 'settings.stoneStyleIndex' ---
            const sf::Texture& texture = tm.getTexture(s, settings.stoneStyleIndex);

            stoneSprite.setTexture(texture);

            float scaleFactor = (STONE_RADIUS * 2.0f) / texture.getSize().x;
            stoneSprite.setScale(scaleFactor, scaleFactor);

            stoneSprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

            stoneSprite.setPosition(toScreenCoord(x, y));

            window.draw(stoneSprite);
        }
    }
}