// src/GameRenderer.cpp
#include "GameRenderer.h"
#include <cmath>
#include <string>

sf::Vector2f GameRenderer::toScreenCoord(int x, int y) {
    return sf::Vector2f(GRID_OFFSET + x * CELL_SIZE, GRID_OFFSET + y * CELL_SIZE);
}

void GameRenderer::drawBoard(sf::RenderTarget& window, const GameSettings& settings, const sf::Font& font) {
    // 1. Draw Background
    sf::RectangleShape fullBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
    fullBackground.setFillColor(sf::Color(60, 60, 60));
    window.draw(fullBackground);

    sf::RectangleShape boardBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
    boardBackground.setFillColor(settings.getBoardColor());
    window.draw(boardBackground);

    // 2. Draw Grid Lines (1 to 19)
    const sf::Color lineColor(0, 0, 0);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        // Vertical line
        sf::Vertex line1[] = {
            sf::Vertex(toScreenCoord(i, 0), lineColor),
            sf::Vertex(toScreenCoord(i, BOARD_SIZE - 1), lineColor)
        };
        window.draw(line1, 2, sf::PrimitiveType::Lines);

        // Horizontal line
        sf::Vertex line2[] = {
            sf::Vertex(toScreenCoord(0, i), lineColor),
            sf::Vertex(toScreenCoord(BOARD_SIZE - 1, i), lineColor)
        };
        window.draw(line2, 2, sf::PrimitiveType::Lines);

        // --- 3. Draw Coordinates (Labels) ---
        
        // Draw Numbers (1-19) on the Left
        // We use i + 1 to map index 0 to "1"
        sf::Text numText(std::to_string(i + 1), font, 14); 
        numText.setFillColor(sf::Color::Black);
        sf::FloatRect nb = numText.getLocalBounds();
        numText.setOrigin(nb.width, nb.height / 2.0f); // Align right
        // Position: slightly to the left of the grid line
        numText.setPosition(toScreenCoord(0, i).x - 15, toScreenCoord(0, i).y);
        window.draw(numText);

        // Draw Letters (A-S) on the Top
        // 'A' is char 65. So 'A' + 0 = A, 'A' + 1 = B...
        char letterChar = 'A' + i;
        sf::Text charText(std::string(1, letterChar), font, 14);
        charText.setFillColor(sf::Color::Black);
        sf::FloatRect cb = charText.getLocalBounds();
        charText.setOrigin(cb.width / 2.0f, cb.height); // Align bottom center
        // Position: slightly above the grid line
        charText.setPosition(toScreenCoord(i, 0).x, toScreenCoord(i, 0).y - 15);
        window.draw(charText);
    }

    // --- 4. Draw Star Points (Hoshi) ---
    // On a 19x19 board, star points are at indices 3, 9, and 15 (0-based)
    // i.e. (4,4), (4,10), (4,16)...
    std::vector<int> starIndices = {3, 9, 15};

    for (int y : starIndices) {
        for (int x : starIndices) {
            sf::CircleShape starPoint(4.0f); // Small dot
            starPoint.setFillColor(sf::Color::Black);
            starPoint.setOrigin(4.0f, 4.0f); // Center it
            starPoint.setPosition(toScreenCoord(x, y));
            window.draw(starPoint);
        }
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