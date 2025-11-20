#include "Definitions.h"
#include "AudioManager.h"
#include "TextureManager.h"
#include "GoGame.h"
#include "GoUIManager.h"
#include "MainMenuScreen.h"
#include "SettingsScreen.h"
#include "GameRenderer.h"
#include "SaveLoadScreen.h"
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

    GameSettings settings;
    AudioManager audioManager(settings);
    StoneTextureManager textureManager;
    GoGame game(audioManager);

    GoUIManager ui(game);
    MainMenuScreen mainMenu;
    SettingsScreen settingsScreen(settings, textureManager, audioManager);

    // --- NEW: Initialize SaveLoadScreen ---
    SaveLoadScreen saveLoadScreen;

    // Load font once to use for all screens
    sf::Font mainFont;
    sf::Font boldFont;
    if (!mainFont.loadFromFile("assets/fonts/arial.ttf")) { /* Error */ }
    if (!boldFont.loadFromFile("assets/fonts/arialbd.ttf")) { /* Error */ }

    saveLoadScreen.init(boldFont, WINDOW_WIDTH, WINDOW_HEIGHT);
    // ---------------------------------------

    // Add SAVE and LOAD to your GameState enum if not already there!
    // Or use existing ones. Let's assume you added them to Definitions.h
    // If not, define them locally or update Definitions.h:
    // enum class GameState { MENU, PLAYING, SETTINGS, SAVE_MENU, LOAD_MENU };
    GameState currentGameState = GameState::MENU;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                // --- 1. MENU STATE ---
                if (currentGameState == GameState::MENU) {
                    currentGameState = mainMenu.handleEvent(event, mousePos, game);

                    // Check if Main Menu triggered Load Game directly
                    // (You might need to add a "Load" button to MainMenu logic)
                    // if (currentGameState == GameState::PLAYING && wantsLoad) {
                    //      currentGameState = GameState::LOAD_MENU;
                    //      saveLoadScreen.setMode(false);
                    // }
                }
                // --- 2. PLAYING STATE ---
                else if (currentGameState == GameState::PLAYING) {
                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        // Handle UI Panel Clicks
                        // Check if user clicked "Save" or "Load" buttons in the side panel
                        // You need to implement these buttons in GoUIManager

                        /* Example Logic:
                        if (ui.isSaveButtonClicked(mousePos)) {
                            currentGameState = GameState::SAVE_MENU;
                            saveLoadScreen.setMode(true); // Save Mode
                        }
                        else if (ui.isLoadButtonClicked(mousePos)) {
                            currentGameState = GameState::LOAD_MENU;
                            saveLoadScreen.setMode(false); // Load Mode
                        }
                        */

                        ui.handleButtonClick(mousePos, window, currentGameState);
                    } else {
                        handleBoardClick(game, mousePos, ui);
                    }
                }
                // --- 3. SAVE/LOAD MENU STATE ---
                else if (currentGameState == GameState::SAVE_MENU || currentGameState == GameState::LOAD_MENU) {
                    int action = saveLoadScreen.handleMouseClick(mousePos);

                    if (action == -2) {
                        // Back button -> Return to Game
                        currentGameState = GameState::PLAYING;
                    }
                    else if (action > 0) {
                        // Slot clicked (1-5)
                        std::string filename = "saves/save_0" + std::to_string(action) + ".txt";

                        if (currentGameState == GameState::SAVE_MENU) {
                            if (game.saveGame(filename)) {
                                ui.setNotification("Game Saved!");
                                saveLoadScreen.refreshSlots(); // Update color to green
                            }
                        }
                        else {
                            if (game.loadGame(filename)) {
                                ui.setNotification("Game Loaded!");
                                currentGameState = GameState::PLAYING; // Go back to game
                            }
                        }
                    }
                }
                // --- 4. SETTINGS STATE ---
                else if (currentGameState == GameState::SETTINGS) {
                    currentGameState = settingsScreen.handleClick(mousePos);
                }
            }

            // Shortcut keys for testing (Ctrl+S = Save Menu, Ctrl+L = Load Menu)
            if (event.type == sf::Event::KeyPressed) {
                if (currentGameState == GameState::PLAYING) {
                    if (event.key.code == sf::Keyboard::S && event.key.control) {
                        currentGameState = GameState::SAVE_MENU;
                        saveLoadScreen.setMode(true);
                    }
                    if (event.key.code == sf::Keyboard::L && event.key.control) {
                        currentGameState = GameState::LOAD_MENU;
                        saveLoadScreen.setMode(false);
                    }
                }
            }
        }

        window.clear();

        // Render Logic
        if (currentGameState == GameState::MENU) {
            mainMenu.draw(window);
        }
        else if (currentGameState == GameState::SETTINGS) {
            settingsScreen.draw(window);
        }
        else if (currentGameState == GameState::PLAYING) {
            GameRenderer::drawBoard(window, settings, boldFont);
            GameRenderer::drawStones(window, game, textureManager, settings);
            ui.draw(window);
        }
        // Render Game BEHIND the Save/Load screen for overlay effect
        else if (currentGameState == GameState::SAVE_MENU || currentGameState == GameState::LOAD_MENU) {
            GameRenderer::drawBoard(window, settings, boldFont);
            GameRenderer::drawStones(window, game, textureManager, settings);
            // Draw the overlay on top
            saveLoadScreen.draw(window);
        }

        window.display();
    }

    return 0;
}