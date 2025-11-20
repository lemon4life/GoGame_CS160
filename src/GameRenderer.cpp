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

        // ---------------------------------------------------------
        // NUMBERS (Rows)
        // ---------------------------------------------------------
        std::string numStr = std::to_string(BOARD_SIZE - i);
        sf::Text numText(numStr, font, 14);
        numText.setFillColor(sf::Color::Black);
        sf::FloatRect nb = numText.getLocalBounds();

        // 1. Draw Numbers on the Left
        numText.setOrigin(nb.width, nb.height / 2.0f); // Align right-center
        numText.setPosition(toScreenCoord(0, i).x - 15, toScreenCoord(0, i).y);
        window.draw(numText);

        // 2. Draw Numbers on the Right (NEW)
        sf::Text rightNumText = numText; // Copy style
        // Set origin to left-center so it draws outwards to the right
        rightNumText.setOrigin(0, nb.height / 2.0f);
        // Position at the X of the last column + offset
        rightNumText.setPosition(toScreenCoord(BOARD_SIZE - 1, i).x + 15, toScreenCoord(BOARD_SIZE - 1, i).y);
        window.draw(rightNumText);

        // ---------------------------------------------------------
        // LETTERS (Columns)
        // ---------------------------------------------------------
        char letterChar = 'A' + i;
        // Optional: Standard Go boards usually skip 'I' to avoid confusion with '1'.
        // If you want that behavior, uncomment the line below:
        // if (letterChar >= 'I') letterChar++;

        sf::Text charText(std::string(1, letterChar), font, 14);
        charText.setFillColor(sf::Color::Black);
        sf::FloatRect cb = charText.getLocalBounds();

        // 3. Draw Letters on the Top
        charText.setOrigin(cb.width / 2.0f, cb.height); // Align bottom-center
        charText.setPosition(toScreenCoord(i, 0).x, toScreenCoord(i, 0).y - 15);
        window.draw(charText);

        // 4. Draw Letters on the Bottom (NEW)
        sf::Text botCharText = charText; // Copy style
        // Set origin to top-center so it draws downwards
        botCharText.setOrigin(cb.width / 2.0f, 0);
        // Position at the Y of the last row + offset
        botCharText.setPosition(toScreenCoord(i, BOARD_SIZE - 1).x, toScreenCoord(i, BOARD_SIZE - 1).y + 15);
        window.draw(botCharText);
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