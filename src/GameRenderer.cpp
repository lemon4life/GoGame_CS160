#include "../include/GameRenderer.h"
#include <iostream>
#include <cmath>
#include <string>

namespace GameRenderer {

    static sf::RenderTexture cachedBoard;
    static sf::Sprite cachedBoardSprite;
    static int cachedThemeIndex = -1;
    static bool cacheIsValid = false;

    sf::Vector2f toScreenCoord(int x, int y) {
        return sf::Vector2f(GRID_OFFSET + x * CELL_SIZE, GRID_OFFSET + y * CELL_SIZE);
    }

    void invalidateCache() {
        cacheIsValid = false;
    }

    void renderGridToCache(const GameSettings& settings, const sf::Font& font) {
        if (cachedBoard.getSize().x != BOARD_MAX_WIDTH || cachedBoard.getSize().y != WINDOW_HEIGHT) {
            cachedBoard.create(BOARD_MAX_WIDTH, WINDOW_HEIGHT);
            cachedBoard.setSmooth(true);
        }

        cachedBoard.clear(sf::Color(60, 60, 60));

        static sf::Texture boardTexture;
        std::string path = settings.getBoardImagePath();
        if (boardTexture.loadFromFile(path)) {
            boardTexture.setSmooth(true);
            sf::RectangleShape boardBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
            boardBackground.setTexture(&boardTexture);
            cachedBoard.draw(boardBackground);
        }

        sf::Color lineColor = (settings.boardThemeIndex == 2) ? sf::Color(220, 220, 220) : sf::Color::Black;
        sf::Color textColor = (settings.boardThemeIndex == 2) ? sf::Color(220, 220, 220) : sf::Color::Black;
        float lineThickness = 2.0f;

        for (int i = 0; i < BOARD_SIZE; ++i) {
            sf::Vector2f vStart = toScreenCoord(i, 0);
            sf::Vector2f vEnd   = toScreenCoord(i, BOARD_SIZE - 1);
            sf::RectangleShape vLine(sf::Vector2f(lineThickness, vEnd.y - vStart.y));
            vLine.setFillColor(lineColor);
            vLine.setPosition(vStart);
            vLine.setOrigin(lineThickness / 2.0f, 0.0f);
            cachedBoard.draw(vLine);

            sf::Vector2f hStart = toScreenCoord(0, i);
            sf::Vector2f hEnd   = toScreenCoord(BOARD_SIZE - 1, i);
            sf::RectangleShape hLine(sf::Vector2f(hEnd.x - hStart.x, lineThickness));
            hLine.setFillColor(lineColor);
            hLine.setPosition(hStart);
            hLine.setOrigin(0.0f, lineThickness / 2.0f);
            cachedBoard.draw(hLine);

            std::string numStr = std::to_string(BOARD_SIZE - i);
            sf::Text numText(numStr, font, 18);
            numText.setFillColor(textColor);
            sf::FloatRect nb = numText.getLocalBounds();
            numText.setOrigin(nb.width, nb.height / 2.0f);
            numText.setPosition(toScreenCoord(0, i).x - 25, toScreenCoord(0, i).y - 5);
            cachedBoard.draw(numText);

            sf::Text rightNumText = numText;
            rightNumText.setOrigin(0, nb.height / 2.0f);
            rightNumText.setPosition(toScreenCoord(BOARD_SIZE - 1, i).x + 25, toScreenCoord(BOARD_SIZE - 1, i).y - 5);
            cachedBoard.draw(rightNumText);

            char letterChar = 'A' + i + (i >= 8);
            sf::Text charText(std::string(1, letterChar), font, 18);
            charText.setFillColor(textColor);
            sf::FloatRect cb = charText.getLocalBounds();
            charText.setOrigin(cb.width / 2.0f, cb.height);
            charText.setPosition(toScreenCoord(i, 0).x, toScreenCoord(i, 0).y - 25);
            cachedBoard.draw(charText);

            sf::Text botCharText = charText;
            botCharText.setOrigin(cb.width / 2.0f, 0);
            botCharText.setPosition(toScreenCoord(i, BOARD_SIZE - 1).x, toScreenCoord(i, BOARD_SIZE - 1).y + 15);
            cachedBoard.draw(botCharText);
        }

        std::vector<int> starIndices = {3, 9, 15};
        for (int y : starIndices) {
            for (int x : starIndices) {
                sf::CircleShape starPoint(4.0f);
                starPoint.setFillColor(lineColor);
                starPoint.setOrigin(4.0f, 4.0f);
                starPoint.setPosition(toScreenCoord(x, y));
                cachedBoard.draw(starPoint);
            }
        }
        cachedBoard.display();
        cachedBoardSprite.setTexture(cachedBoard.getTexture());
        cacheIsValid = true;
        cachedThemeIndex = settings.boardThemeIndex;
    }

    void drawBoard(sf::RenderWindow& window, const GameSettings& settings, const sf::Font& font) {
        if (!cacheIsValid || cachedThemeIndex != settings.boardThemeIndex) {
            renderGridToCache(settings, font);
        }
        window.draw(cachedBoardSprite);
    }

    void drawStones(sf::RenderWindow& window, const GoGame& game, const StoneTextureManager& tm, const GameSettings& settings) {
        static sf::Sprite stoneSprite;

        // Pre-fetch texture pointers to avoid lookups
        const sf::Texture* blackTex = &tm.getTexture(Stone::Black, settings.stoneStyleIndex);
        const sf::Texture* whiteTex = &tm.getTexture(Stone::White, settings.stoneStyleIndex);

        // We use a pointer to track the current texture so we don't re-set it unnecessarily
        const sf::Texture* currentTex = nullptr;

        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                Stone s = game.getStoneAt(x, y);
                if (s == Stone::Empty) continue;

                // 1. Determine which texture to use
                const sf::Texture* neededTex = (s == Stone::White) ? blackTex : whiteTex;

                // 2. Only update sprite properties if texture changed
                if (currentTex != neededTex) {
                    currentTex = neededTex;
                    stoneSprite.setTexture(*currentTex,true);

                    // --- FIX: Calculate Scale & Origin based on THIS texture's size ---
                    sf::Vector2u texSize = currentTex->getSize();

                    // Origin should be the center of the image
                    stoneSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);

                    // Scale should fit the image into the cell (STONE_RADIUS * 2)
                    float scaleFactor = (STONE_RADIUS * 2.0f) / texSize.x;
                    stoneSprite.setScale(scaleFactor, scaleFactor);
                }

                stoneSprite.setPosition(toScreenCoord(x, y));
                window.draw(stoneSprite);
            }
        }

        // --- GHOST STONE LOGIC ---
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (mousePos.x < BOARD_MAX_WIDTH) {
            int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
            int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

            if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
                auto validMoves = game.getValidMoves();
                if (validMoves[gx + 1][gy + 1]) {
                    Stone currentPlayer = game.getCurrentPlayer();

                    // Determine ghost texture
                    const sf::Texture* ghostTex = (currentPlayer == Stone::Black) ? blackTex : whiteTex;

                    // Apply and recalculate scale/origin for ghost
                    stoneSprite.setTexture(*ghostTex, true);

                    sf::Vector2u texSize = ghostTex->getSize();
                    stoneSprite.setOrigin(texSize.x / 2.0f, texSize.y / 2.0f);
                    float scaleFactor = (STONE_RADIUS * 2.0f) / texSize.x;
                    stoneSprite.setScale(scaleFactor, scaleFactor);

                    stoneSprite.setColor(sf::Color(255, 255, 255, 128)); // Transparent
                    stoneSprite.setPosition(toScreenCoord(gx, gy));

                    window.draw(stoneSprite);

                    // Reset color for next frame
                    stoneSprite.setColor(sf::Color::White);

                    // Reset tracker so loop doesn't assume wrong state next time
                    currentTex = nullptr;
                }
            }
        }
    }
}