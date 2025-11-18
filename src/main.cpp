#include "Definitions.h"
#include "AudioManager.h"
#include "TextureManager.h"
#include "GoGame.h"
#include "GoUIManager.h"
#include "MainMenuScreen.h"
#include "SettingsScreen.h"
#include "GameRenderer.h"
#include <cmath>

void handleBoardClick(GoGame& game, const sf::Vector2i& mousePos, GoUIManager& ui) {
    if (mousePos.x >= BOARD_MAX_WIDTH) return;
    int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
    int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
        if (game.placeStone(gx, gy)) {
            ui.setNotification("Stone Placed");
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Go Game Ultimate");
    window.setFramerateLimit(60);

    // 1. Initialize Systems
    GameSettings settings;
    AudioManager audioManager(settings);
    StoneTextureManager textureManager;

    GoGame game(audioManager);

    // 2. Initialize Screens
    GoUIManager ui(game);
    MainMenuScreen mainMenu;
    SettingsScreen settingsScreen(settings, textureManager, audioManager);

    GameState currentGameState = GameState::MENU;
    sf::Event event;

    sf::Font mainFont;
    if (!mainFont.loadFromFile("assets/fonts/arial.ttf")) {
        // handle error
    }

    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                if (currentGameState == GameState::MENU) {
                    currentGameState = mainMenu.handleEvent(event, mousePos, game);
                }
                else if (currentGameState == GameState::SETTINGS) {
                    currentGameState = settingsScreen.handleClick(mousePos);
                }
                else if (currentGameState == GameState::PLAYING) {
                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        ui.handleButtonClick(mousePos, window, currentGameState);
                    } else {
                        handleBoardClick(game, mousePos, ui);
                    }
                }
            }
        }

        window.clear();

        if (currentGameState == GameState::MENU) {
            mainMenu.draw(window);
        }
        else if (currentGameState == GameState::SETTINGS) {
            settingsScreen.draw(window);
        }
        else if (currentGameState == GameState::PLAYING) {
            // UPDATE THIS LINE: Pass 'mainFont'
            GameRenderer::drawBoard(window, settings, mainFont);

            GameRenderer::drawStones(window, game, textureManager, settings);
            ui.draw(window);
        }

        window.display();
    }

    return 0;
}