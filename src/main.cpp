#include "Definitions.h"
#include "AudioManager.h"
#include "TextureManager.h"
#include "GoGame.h"
#include "GoUIManager.h"
#include "MainMenuScreen.h"
#include "SettingsScreen.h"
#include "GameRenderer.h"
#include "SaveLoadScreen.h"
#include "GameOverScreen.h"
#include "ToggleScreen.h"
#include <cmath>
#include <iomanip>
#include <sstream>

void handleBoardClick(GoGame& game, const sf::Vector2i& mousePos, GoUIManager& ui) {
    if (mousePos.x >= BOARD_MAX_WIDTH) return;
    int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
    int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
        int action = game.placeStone(gx, gy);
        if (action == 1) {
            ui.setNotification("Stone Placed");
        } else if (action == -1) {
            if (game.getCurrentPlayer() == Stone::Black)
                ui.setNotification("Black captured!");
            else
                ui.setNotification(("White captured!"));
        } else {
            ui.setNotification(("Illegal move!"));
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Go Game Ultimate");
    window.setFramerateLimit(60);

    // 1. Init Systems
    GameSettings settings;
    AudioManager audioManager(settings);
    StoneTextureManager textureManager;
    GoGame game(audioManager);

    // 2. Init Screens
    GoUIManager ui(game);
    MainMenuScreen mainMenu;
    SettingsScreen settingsScreen(settings, textureManager, audioManager);
    SaveScreen saveScreen;
    LoadScreen loadScreen;
    GameOverScreen gameOverScreen;
    ToggleScreen toggleScreen;

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arialbd.ttf")) {
        cerr << "Error Loading Fonts!";
    }

    saveScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "SAVE GAME");
    loadScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "LOAD GAME");
    gameOverScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);
    toggleScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);

    GameState currentGameState = GameState::MENU;
    bool whilePlaying = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // --- MOUSE MOVE ---
            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2i mPos(event.mouseMove.x, event.mouseMove.y);
                if (currentGameState == GameState::SAVE_MENU) saveScreen.handleMouseMove(mPos);
                else if (currentGameState == GameState::LOAD_MENU) loadScreen.handleMouseMove(mPos);
            }

            // --- MOUSE CLICK ---
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                // --- MENU ---
                if (currentGameState == GameState::EXIT) { window.close(); }
                if (currentGameState == GameState::MENU) {
                    whilePlaying = false;
                    GameState newState = mainMenu.handleEvent(event, mousePos, game);
                    if (newState == GameState::LOAD_MENU) loadScreen.refreshSlots();
                    currentGameState = newState;
                }
                // --- SETTINGS ---
                else if (currentGameState == GameState::SETTINGS) {
                    if (settingsScreen.handleClick(mousePos))
                        currentGameState = (whilePlaying ? GameState::PLAYING : GameState::MENU);
                }
                // --- PLAYING ---
                else if (currentGameState == GameState::PLAYING) {
                    whilePlaying = true;
                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        ui.handleButtonClick(mousePos, window, currentGameState);

                        // Transition Logic
                        if (currentGameState == GameState::GAME_OVER) {
                            // Pass button clicked 2x -> Go to Scoring
                            currentGameState = GameState::SCORING;
                            // Run heuristic initially? game.runHeuristic();
                        }
                        else if (currentGameState == GameState::SAVE_MENU) {
                            saveScreen.reload();
                            saveScreen.refreshSlots();
                        }

                        else if (currentGameState == GameState::LOAD_MENU) {
                            loadScreen.reload();
                            loadScreen.refreshSlots();
                        }
                    } else {
                        handleBoardClick(game, mousePos, ui);
                    }
                }
                // --- SCORING (Toggle Dead Stones) ---
                else if (currentGameState == GameState::SCORING) {
                    // Delegate to ToggleScreen
                    bool finished = toggleScreen.handleClick(mousePos, game);

                    if (finished) {
                        currentGameState = GameState::GAME_OVER; // Go to Popup

                        // Calculate Final Score
                        auto scores = game.getScore();
                        float bScore = scores.first;
                        float wScore = scores.second;

                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(1)
                           << "Black: " << bScore << "   White: " << wScore;

                        std::string msg;
                        if (bScore > wScore) msg = "Black Wins!";
                        else if (wScore > bScore) msg = "White Wins!";
                        else msg = "Draw!";

                        gameOverScreen.setGameOverMessage(msg, ss.str());
                    }
                }
                // --- GAME OVER POPUP ---
                else if (currentGameState == GameState::GAME_OVER) {
                    std::string action = gameOverScreen.handleMouseClick(mousePos);
                    if (action == "NEW") {
                        game.resetGame();
                        currentGameState = GameState::PLAYING;
                    } else if (action == "LOAD") {
                        loadScreen.refreshSlots();
                        currentGameState = GameState::LOAD_MENU;
                    } else if (action == "EXIT") {
                        currentGameState = GameState::MENU;
                    }
                }
                // --- SAVE/LOAD ---
                else if (currentGameState == GameState::SAVE_MENU) {
                    int action = saveScreen.handleMouseClick(mousePos);
                    if (action == -2) currentGameState = whilePlaying ? GameState::PLAYING : GameState::MENU;
                    else if (action > 0) {
                        if (game.saveGame("saves/save_0" + std::to_string(action) + ".txt")) {
                            ui.setNotification("Game Saved!");

                            saveScreen.refreshSlots();
                        }

                        currentGameState = GameState::PLAYING;
                    }
                }
                else if (currentGameState == GameState::LOAD_MENU) {
                    int action = loadScreen.handleMouseClick(mousePos);
                    if (action == -2) {
                        if (action == -2) currentGameState = whilePlaying ? GameState::PLAYING : GameState::MENU;
                    } else if (action > 0) {
                        if (game.loadGame("saves/save_0" + std::to_string(action) + ".txt")) {
                            ui.setNotification("Game Loaded!");
                            currentGameState = GameState::PLAYING;
                        }
                    }
                }
            }
        }

        window.clear();

        if (currentGameState == GameState::EXIT) {
            window.close();
        }

        // --- RENDER ---
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
        else if (currentGameState == GameState::SCORING) {
            // Draw Board (Normal stones, NO markers from renderer)
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);

            // Draw UI (Side panel hides old buttons)
            ui.draw(window); // Draw base sidebar

            // Draw Toggle Screen on top (Finish button + Dead markers)
            toggleScreen.draw(window, game);
        }
        else if (currentGameState == GameState::GAME_OVER) {
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);
            ui.draw(window);
            gameOverScreen.draw(window);
        }
        else if (currentGameState == GameState::SAVE_MENU) {
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);
            saveScreen.refreshSlots();
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