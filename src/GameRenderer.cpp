// src/GameRenderer.cpp
#include "GameRenderer.h"
#include <cmath>
#include <string>

sf::Vector2f GameRenderer::toScreenCoord(int x, int y) {
    return sf::Vector2f(GRID_OFFSET + x * CELL_SIZE, GRID_OFFSET + y * CELL_SIZE);
}

#include "GameRenderer.h"
#include <iostream> // For error logging

namespace GameRenderer {

    // ... (Keep your toScreenCoord function here) ...

    void drawBoard(sf::RenderWindow& window, const GameSettings& settings, const sf::Font& font) {

        // --- 1. Texture Caching Logic (The "Namespace" Fix) ---
        // 'static' means these variables persist between function calls!
        static sf::Texture boardTexture;
        static int lastThemeIndex = -1;

        // Only load the image if the theme changed (or on first run)
        if (lastThemeIndex != settings.boardThemeIndex) {
            std::string path = settings.getBoardImagePath();
            if (boardTexture.loadFromFile(path)) {
                boardTexture.setSmooth(true);
                lastThemeIndex = settings.boardThemeIndex;
                std::cout << "[GameRenderer] Loaded board texture: " << path << std::endl;
            } else {
                std::cerr << "[GameRenderer] Failed to load texture: " << path << std::endl;
            }
        }

        // --- 2. Draw Background ---

        // Dark grey background for the whole window
        sf::RectangleShape fullBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
        fullBackground.setFillColor(sf::Color(60, 60, 60));
        window.draw(fullBackground);

        // The Board Area with Texture
        sf::RectangleShape boardBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
        boardBackground.setTexture(&boardTexture); // Apply the static texture
        boardBackground.setFillColor(sf::Color::White); // White so it doesn't tint the image
        window.draw(boardBackground);

        // --- 3. Determine Line/Text Colors ---
        // If theme is Dark (index 2), use White lines. Otherwise, use Black.
        sf::Color lineColor = (settings.boardThemeIndex == 2) ? sf::Color(220, 220, 220) : sf::Color::Black;
        sf::Color textColor = (settings.boardThemeIndex == 2) ? sf::Color(220, 220, 220) : sf::Color::Black;

        float lineThickness = 2.0f;

        // --- 4. Draw Grid Lines ---
        for (int i = 0; i < BOARD_SIZE; ++i) {
            // ----------------------------------------
            // Vertical Line
            // ----------------------------------------
            sf::Vector2f vStart = toScreenCoord(i, 0);
            sf::Vector2f vEnd   = toScreenCoord(i, BOARD_SIZE - 1);

            // Calculate height based on coordinates
            float height = vEnd.y - vStart.y;

            sf::RectangleShape verticalLine(sf::Vector2f(lineThickness, height));
            verticalLine.setFillColor(lineColor);
            verticalLine.setPosition(vStart);

            // Center the thickness so the line sits exactly on the grid coordinate
            // Origin (thickness/2, 0) shifts the drawing slightly left
            verticalLine.setOrigin(lineThickness / 2.0f, 0.0f);

            window.draw(verticalLine);

            // ----------------------------------------
            // Horizontal Line
            // ----------------------------------------
            sf::Vector2f hStart = toScreenCoord(0, i);
            sf::Vector2f hEnd   = toScreenCoord(BOARD_SIZE - 1, i);

            // Calculate width based on coordinates
            float width = hEnd.x - hStart.x;

            sf::RectangleShape horizontalLine(sf::Vector2f(width, lineThickness));
            horizontalLine.setFillColor(lineColor);
            horizontalLine.setPosition(hStart);

            // Center the thickness vertically
            // Origin (0, thickness/2) shifts the drawing slightly up
            horizontalLine.setOrigin(0.0f, lineThickness / 2.0f);

            window.draw(horizontalLine);

            // --- 5. Draw Coordinates (Labels) ---

            // ---------------- Numbers (Rows) ----------------
            std::string numStr = std::to_string(BOARD_SIZE - i);
            sf::Text numText(numStr, font, 20);
            numText.setFillColor(textColor);
            sf::FloatRect nb = numText.getLocalBounds();

            // Left Side
            numText.setOrigin(nb.width, nb.height / 2.0f);
            numText.setPosition(toScreenCoord(0, i).x - 25, toScreenCoord(0, i).y);
            window.draw(numText);

            // Right Side
            sf::Text rightNumText = numText;
            rightNumText.setOrigin(0, nb.height / 2.0f);
            rightNumText.setPosition(toScreenCoord(BOARD_SIZE - 1, i).x + 25, toScreenCoord(BOARD_SIZE - 1, i).y);
            window.draw(rightNumText);

            // ---------------- Letters (Columns) ----------------
            char letterChar = 'A' + i + (i >= 8);
            // if (letterChar >= 'I') letterChar++; // Uncomment for standard Go skipping 'I'

            sf::Text charText(std::string(1, letterChar), font, 20);
            charText.setFillColor(textColor);
            sf::FloatRect cb = charText.getLocalBounds();

            // Top Side
            charText.setOrigin(cb.width / 2.0f, cb.height);
            charText.setPosition(toScreenCoord(i, 0).x, toScreenCoord(i, 0).y - 30);
            window.draw(charText);

            // Bottom Side
            sf::Text botCharText = charText;
            botCharText.setOrigin(cb.width / 2.0f, 0);
            botCharText.setPosition(toScreenCoord(i, BOARD_SIZE - 1).x, toScreenCoord(i, BOARD_SIZE - 1).y + 25);
            window.draw(botCharText);
        }

        // --- 6. Draw Star Points (Hoshi) ---
        std::vector<int> starIndices = {3, 9, 15};
        for (int y : starIndices) {
            for (int x : starIndices) {
                sf::CircleShape starPoint(6.0f);
                starPoint.setFillColor(lineColor);
                starPoint.setOrigin(6.0f, 6.0f);
                starPoint.setPosition(toScreenCoord(x, y));
                window.draw(starPoint);
            }
        }
    }
}
void GameRenderer::drawStones(sf::RenderWindow& window, const GoGame& game, const StoneTextureManager& tm, const GameSettings& settings) {
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

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

    // Only draw if mouse is within the board area
    if (mousePos.x < BOARD_MAX_WIDTH) {
        int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
        int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

    // Check bounds
    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
    // Get valid moves matrix from Engine
    // You need to expose validMoves via GoGame first!
    // Assuming GoGame has: std  ::vector<std::vector<bool>> getValidMoves() const;
    auto validMoves = game.getValidMoves();

        cerr << "Reset!\n";
        cerr << "Reset!\n";
        cerr << "Reset!\n";
        cerr << "Reset!\n";
        cerr << "Reset!\n";
        cerr << "Reset!\n";

    // Note: Engine uses 1-based indexing, vector might be size 20x20
    // Or if your validMoves() returns 0-based 19x19, adjust accordingly.
    // Based on your Board.cpp snippet, it returns size [boardSize+1][boardSize+1].

    if (validMoves[gx + 1][gy + 1]) {
        sf::Sprite ghostSprite;

        cout << "valid\n";

        // Determine current player to show correct color ghost
        Stone currentPlayer = game.getCurrentPlayer();
        if (currentPlayer == Stone::White) {
            ghostSprite.setTexture(tm.getTexture(Stone::Black, settings.stoneStyleIndex));
        } else {
            ghostSprite.setTexture(tm.getTexture(Stone::White, settings.stoneStyleIndex));
        }

        // Set transparency (alpha = 128 is ~50%)
        ghostSprite.setColor(sf::Color(255, 255, 255, 128));

        sf::FloatRect bounds = ghostSprite.getLocalBounds();
        ghostSprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        ghostSprite.setPosition(toScreenCoord(gx, gy));

        float scale = (CELL_SIZE * 0.95f) / bounds.width;
        ghostSprite.setScale(scale, scale);

        window.draw(ghostSprite);
    }
    }
    }
}