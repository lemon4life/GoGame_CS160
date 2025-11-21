#include "Definitions.h"
#include "AudioManager.h"
#include "TextureManager.h"
#include "GoGame.h"
#include "GoUIManager.h"
#include "MainMenuScreen.h"
#include "SettingsScreen.h"
#include "GameRenderer.h"
#include "SaveLoadScreen.h" // <--- Just this one header
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

    // 1. Init Game Systems
    GameSettings settings;
    AudioManager audioManager(settings);
    StoneTextureManager textureManager;
    GoGame game(audioManager);

    // 2. Init UI Screens
    GoUIManager ui(game);
    MainMenuScreen mainMenu;
    SettingsScreen settingsScreen(settings, textureManager, audioManager);

    // 3. Init Save & Load Screens
    SaveScreen saveScreen;
    LoadScreen loadScreen;

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arialbd.ttf")) {
        // Handle Error
    }

    saveScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "SAVE GAME");
    loadScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "LOAD GAME");

    GameState currentGameState = GameState::MENU;
    bool whilePlaying = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // --- MOUSE MOVE ---
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mousePos(event.mouseMove.x, event.mouseMove.y);
                if (currentGameState == GameState::SAVE_MENU) {
                    saveScreen.handleMouseMove(mousePos);
                } else if (currentGameState == GameState::LOAD_MENU) {
                    loadScreen.handleMouseMove(mousePos);
                }
            }

            // --- MOUSE CLICK ---
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                // --- STATE: MAIN MENU ---
                if (currentGameState == GameState::MENU) {
                    GameState newState = mainMenu.handleEvent(event, mousePos, game);
                    if (newState == GameState::LOAD_MENU) {
                        loadScreen.refreshSlots();
                    }
                    currentGameState = newState;
                }
                // --- STATE: SETTINGS ---
                else if (currentGameState == GameState::SETTINGS){
                    if (settingsScreen.handleClick(mousePos)) {
                        currentGameState = (whilePlaying ? GameState::PLAYING : GameState::MENU);
                    }

                }
                // --- STATE: PLAYING ---
                else if (currentGameState == GameState::PLAYING) {
                    whilePlaying = true;
                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        // Check Sidebar Clicks
                        GameState oldState = currentGameState;
                        ui.handleButtonClick(mousePos, window, currentGameState);

                        if (currentGameState == GameState::SAVE_MENU) saveScreen.refreshSlots();
                        if (currentGameState == GameState::LOAD_MENU) loadScreen.refreshSlots();
                    } else {
                        handleBoardClick(game, mousePos, ui);
                    }
                }
                // --- STATE: SAVE MENU ---
                else if (currentGameState == GameState::SAVE_MENU) {
                    int action = saveScreen.handleMouseClick(mousePos);

                    if (action == -2) {
                        currentGameState = GameState::PLAYING;
                    }
                    else if (action > 0) {
                        std::string filename = "saves/save_0" + std::to_string(action) + ".txt";
                        currentGameState = GameState::PLAYING;
                        if (game.saveGame(filename)) {
                            ui.setNotification("Game Saved!");
                            saveScreen.refreshSlots();
                        }
                    }
                }
                // --- STATE: LOAD MENU ---
                else if (currentGameState == GameState::LOAD_MENU) {
                    int action = loadScreen.handleMouseClick(mousePos);

                    if (action == -2) {
                        if (game.getCurrentPlayer() == Stone::Empty) currentGameState = GameState::MENU;
                        else currentGameState = GameState::PLAYING;
                    }
                    else if (action > 0) {
                        std::string filename = "saves/save_0" + std::to_string(action) + ".txt";
                        if (game.loadGame(filename)) {
                            ui.setNotification("Game Loaded!");
                            currentGameState = GameState::PLAYING;
                        }
                    }
                }
            }

            // --- KEYBOARD SHORTCUTS ---
            if (event.type == sf::Event::KeyPressed && currentGameState == GameState::PLAYING) {
                if (event.key.code == sf::Keyboard::S && event.key.control) {
                    currentGameState = GameState::SAVE_MENU;
                    saveScreen.refreshSlots();
                }
                if (event.key.code == sf::Keyboard::L && event.key.control) {
                    currentGameState = GameState::LOAD_MENU;
                    loadScreen.refreshSlots();
                }
            }
        }

        window.clear();

        // --- RENDER LOGIC ---

        if (currentGameState == GameState::MENU) {
            mainMenu.draw(window);
        }
        else if (currentGameState == GameState::SETTINGS) {
            settingsScreen.draw(window);
        }
        else if (currentGameState == GameState::PLAYING) {
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);
            ui.draw(window);
        }
        // Draw Overlays
        else if (currentGameState == GameState::SAVE_MENU) {
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);
            saveScreen.draw(window, textureManager);
        }
        else if (currentGameState == GameState::LOAD_MENU) {
            if (game.getCurrentPlayer() != Stone::Empty) {
                GameRenderer::drawBoard(window, settings, font);
                GameRenderer::drawStones(window, game, textureManager, settings);
            } else {
                mainMenu.draw(window);
            }
            loadScreen.draw(window, textureManager);
        }

        window.display();
    }

    return 0;
}