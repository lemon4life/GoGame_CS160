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
#include "NewGameScreen.h" // <--- Added NewGameScreen
#include <cmath>
#include <iomanip>
#include <sstream>

// Helper to handle clicks on the board
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
                ui.setNotification("White captured!");
        } else {
            ui.setNotification("Illegal move!");
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
    NewGameScreen newGameScreen; // <--- Init New Game Screen

    sf::Font font;
    if (!font.loadFromFile("assets/fonts/arialbd.ttf")) {
        // Fallback if bold is missing
        if (!font.loadFromFile("assets/fonts/arial.ttf")) {
            std::cerr << "Error Loading Fonts!";
        }
    }

    // 3. Initialize Screen Resources
    saveScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "SAVE GAME");
    loadScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "LOAD GAME");
    gameOverScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);
    toggleScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);
    newGameScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT); // <--- Init

    GameState currentGameState = GameState::MENU;
    bool whilePlaying = false; // Tracks if we came from gameplay (for settings/save menu return)

    while (window.isOpen()) {
        // --- AI UPDATE ---
        // Keeps AI alive without freezing UI (if threaded)
        if (currentGameState == GameState::PLAYING) {
            game.updateAI();
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // --- MOUSE CLICK HANDLING ---
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                // -------------------------------------------------
                // STATE: EXIT
                // -------------------------------------------------
                if (currentGameState == GameState::EXIT) { window.close(); }

                // -------------------------------------------------
                // STATE: MAIN MENU
                // -------------------------------------------------
                else if (currentGameState == GameState::MENU) {
                    whilePlaying = false;
                    GameState newState = mainMenu.handleEvent(event, mousePos, game);

                    if (newState == GameState::NEW_GAME_MENU) {
                        currentGameState = GameState::NEW_GAME_MENU;
                    }
                    else if (newState == GameState::LOAD_MENU) {
                        loadScreen.refreshSlots();
                        currentGameState = GameState::LOAD_MENU;
                    }
                    else if (newState == GameState::SETTINGS) {
                        currentGameState = GameState::SETTINGS;
                    }
                    else if (newState == GameState::EXIT) {
                        window.close();
                    }
                }

                // -------------------------------------------------
                // STATE: NEW GAME SELECTION
                // -------------------------------------------------
                else if (currentGameState == GameState::NEW_GAME_MENU) {
                    std::string action = newGameScreen.handleMouseClick(mousePos);

                    if (action == "BACK") {
                        currentGameState = GameState::MENU;
                    }
                    else if (action != "") {
                        // 1. Reset Board
                        game.resetGame();

                        // 2. Configure Mode
                        if (action == "PVP") {
                            game.setGameMode(GameMode::PVP);
                            game.setAIDifficulty(AIDifficulty::None);
                        }
                        else {
                            game.setGameMode(GameMode::AI);
                            // Ensure AI Engine is ready
                            game.initAI("katago.exe", "model.bin.gz", "analysis_example.cfg");

                            if (action == "AI_EASY") game.setAIDifficulty(AIDifficulty::Easy);
                            else if (action == "AI_MEDIUM") game.setAIDifficulty(AIDifficulty::Medium);
                            else if (action == "AI_HARD") game.setAIDifficulty(AIDifficulty::Hard);
                        }

                        // 3. Start Playing
                        currentGameState = GameState::PLAYING;
                    }
                }

                // -------------------------------------------------
                // STATE: SETTINGS
                // -------------------------------------------------
                else if (currentGameState == GameState::SETTINGS) {
                    if (settingsScreen.handleClick(mousePos))
                        currentGameState = (whilePlaying ? GameState::PLAYING : GameState::MENU);
                }

                // -------------------------------------------------
                // STATE: PLAYING
                // -------------------------------------------------
                else if (currentGameState == GameState::PLAYING) {
                    whilePlaying = true;
                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        // Handle Side UI Buttons
                        ui.handleButtonClick(mousePos, window, currentGameState);

                        // UI Logic Transitions
                        if (currentGameState == GameState::GAME_OVER) {
                            // --- GAME OVER LOGIC ---
                            auto scores = game.getScore();
                            float bScore = scores.first;
                            float wScore = scores.second;

                            std::stringstream ss;
                            ss << std::fixed << std::setprecision(1)
                               << "Black: " << bScore << "   White: " << wScore;

                            std::string msg;
                            // Smart Message based on Mode
                            if (game.getGameMode() == GameMode::AI) {
                                if (bScore > wScore) msg = "YOU WIN!";
                                else if (wScore > bScore) msg = "AI WINS!";
                                else msg = "Draw!";
                            } else {
                                if (bScore > wScore) msg = "Black Wins!";
                                else if (wScore > bScore) msg = "White Wins!";
                                else msg = "Draw!";
                            }

                            gameOverScreen.setGameOverMessage(msg, ss.str());
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
                        // Handle Board Clicks
                        handleBoardClick(game, mousePos, ui);
                    }
                }

                // -------------------------------------------------
                // STATE: GAME OVER
                // -------------------------------------------------
                else if (currentGameState == GameState::GAME_OVER) {
                    std::string action = gameOverScreen.handleMouseClick(mousePos);

                    if (action == "NEW") {
                        // Go to Selection Screen instead of immediate reset
                        currentGameState = GameState::NEW_GAME_MENU;
                    }
                    else if (action == "LOAD") {
                        loadScreen.refreshSlots();
                        currentGameState = GameState::LOAD_MENU;
                    }
                    else if (action == "EXIT") {
                        currentGameState = GameState::MENU;
                    }
                }

                // -------------------------------------------------
                // STATE: SAVE MENU
                // -------------------------------------------------
                else if (currentGameState == GameState::SAVE_MENU) {
                    int action = saveScreen.handleMouseClick(mousePos);
                    if (action == -2) {
                        currentGameState = whilePlaying ? GameState::PLAYING : GameState::MENU;
                    }
                    else if (action > 0) {
                        // Pass Save Folder Path
                        if (game.saveGame("saves/save_0" + std::to_string(action) + ".txt")) {
                            ui.setNotification("Game Saved!");
                            saveScreen.refreshSlots();
                        }
                        currentGameState = GameState::PLAYING;
                    }
                }

                // -------------------------------------------------
                // STATE: LOAD MENU
                // -------------------------------------------------
                else if (currentGameState == GameState::LOAD_MENU) {
                    int action = loadScreen.handleMouseClick(mousePos);
                    if (action == -2) {
                        currentGameState = whilePlaying ? GameState::PLAYING : GameState::MENU;
                    } else if (action > 0) {
                        // Load Game
                        if (game.loadGame("saves/save_0" + std::to_string(action) + ".txt")) {
                            ui.setNotification("Game Loaded!");
                            currentGameState = GameState::PLAYING;
                            whilePlaying = true;
                        }
                    }
                }
            }
        }

        window.clear();

        // --- RENDER LOGIC ---
        if (currentGameState == GameState::MENU) {
            mainMenu.draw(window);
        }
        else if (currentGameState == GameState::NEW_GAME_MENU) {
            mainMenu.draw(window); // Draw Main Menu bg behind it
            newGameScreen.draw(window);
        }
        else if (currentGameState == GameState::SETTINGS) {
            settingsScreen.draw(window);
        }
        else if (currentGameState == GameState::PLAYING ||
                 currentGameState == GameState::GAME_OVER ||
                 currentGameState == GameState::SAVE_MENU ||
                 currentGameState == GameState::LOAD_MENU)
        {
            // Always draw board and stones in background for these states
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);

            // Draw UI Sidebar
            ui.draw(window);

            // Draw Overlays
            if (currentGameState == GameState::GAME_OVER) {
                gameOverScreen.draw(window);
            }
            else if (currentGameState == GameState::SAVE_MENU) {
                saveScreen.draw(window, textureManager);
            }
            else if (currentGameState == GameState::LOAD_MENU) {
                loadScreen.draw(window, textureManager);
            }
        }

        window.display();
    }
    return 0;
}