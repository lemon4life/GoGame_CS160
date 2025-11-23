#include "ToggleScreen.h"
#include <cmath>

void ToggleScreen::init(const sf::Font& font, float width, float height) {
    // 1. Sidebar Cover (Hides Undo/Redo/etc)
    // Positioned where the buttons usually start
    sidebarCover.setSize(sf::Vector2f(PANEL_WIDTH, height - BUTTON_START_Y_GAME_UI + 50));
    sidebarCover.setPosition(width - PANEL_WIDTH, BUTTON_START_Y_GAME_UI - 40);
    sidebarCover.setFillColor(sf::Color(40, 40, 40)); // Match panel color

    // 2. Finish Button
    finishBtn.setSize(sf::Vector2f(PANEL_WIDTH - 40, 60));
    finishBtn.setPosition(width - PANEL_WIDTH + 20, height / 2.0f);
    finishBtn.setFillColor(sf::Color(50, 150, 50)); // Green
    finishBtn.setOutlineThickness(2);
    finishBtn.setOutlineColor(sf::Color::White);

    finishBtnText.setFont(font);
    finishBtnText.setString("FINISH");
    finishBtnText.setCharacterSize(20);
    finishBtnText.setFillColor(sf::Color::White);
    
    // Center Text
    sf::FloatRect tr = finishBtnText.getLocalBounds();
    finishBtnText.setOrigin(tr.left + tr.width/2.0f, tr.top + tr.height/2.0f);
    finishBtnText.setPosition(
        finishBtn.getPosition().x + finishBtn.getSize().x/2.0f,
        finishBtn.getPosition().y + finishBtn.getSize().y/2.0f
    );

    // 3. Instructions
    instructionText.setFont(font);
    instructionText.setString("Click stones to\ntoggle them between\nDEAD and ALIVE.");
    instructionText.setCharacterSize(18);
    instructionText.setFillColor(sf::Color::Yellow);
    instructionText.setPosition(width - PANEL_WIDTH + 10, BUTTON_START_Y_GAME_UI);

    // 4. Init Marker Shape (Red Square)
    float markerSize = CELL_SIZE * 0.5f;
    deadMarker.setSize(sf::Vector2f(markerSize, markerSize));
    deadMarker.setOrigin(markerSize/2.0f, markerSize/2.0f);
    deadMarker.setFillColor(sf::Color(255, 0, 0, 180)); // Semi-transparent Red
}

void ToggleScreen::draw(sf::RenderWindow& window, GoGame& game) {
    // 1. Draw UI Panel over old buttons
    window.draw(sidebarCover);
    window.draw(instructionText);
    window.draw(finishBtn);
    window.draw(finishBtnText);

    // 2. Draw Dead Markers
    // We fetch the dead state directly from game
    std::vector<std::vector<bool>> deadStones = game.getDeadStones();

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            // Note: Ensure coordinate mapping matches your Board.cpp logic
            // If deadStones[x][y] is true, draw marker at grid(x, y)
            if (deadStones[x + 1][y + 1]) {
                //cout << x << ' ' << y << '\n';
                // Convert Grid Index -> Screen Pixel
                float screenX = GRID_OFFSET + x * CELL_SIZE;
                float screenY = GRID_OFFSET + y * CELL_SIZE;
                
                deadMarker.setPosition(screenX, screenY);
                window.draw(deadMarker);
            }
        }
    }
}

bool ToggleScreen::handleClick(const sf::Vector2i& mousePos, GoGame& game) {
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    // 1. Check Finish Button
    if (finishBtn.getGlobalBounds().contains(mx, my)) {
        return true; // Signal to finish
    }

    // 2. Check Board Clicks (Toggle Logic)
    if (mx < BOARD_MAX_WIDTH) {
        int gx = std::round((mx - GRID_OFFSET) / CELL_SIZE);
        int gy = std::round((my - GRID_OFFSET) / CELL_SIZE);

        if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
            // Only toggle if there is actually a stone there? 
            // Usually engine handles that check, but safe to call anyway.
            if (game.getStoneAt(gx, gy) != Stone::Empty) {
                game.toggleDeadStone(gx, gy);
            }
        }
    }

    return false;
}