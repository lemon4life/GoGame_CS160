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
#include "NewGameScreen.h"
#include "Theme.h"
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
    GoUIManager ui(game);

    // 2. Init Screens
    MainMenuScreen mainMenu;
    SettingsScreen settingsScreen(settings, textureManager, audioManager);
    SaveScreen saveScreen;
    LoadScreen loadScreen;
    GameOverScreen gameOverScreen;
    ToggleScreen toggleScreen;
    NewGameScreen newGameScreen;

    sf::Font font;
    font.loadFromFile("assets/fonts/arialbd.ttf");

    // Init with fonts
    saveScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "SAVE GAME");
    loadScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT, "LOAD GAME");
    gameOverScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);
    toggleScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);
    newGameScreen.init(font, WINDOW_WIDTH, WINDOW_HEIGHT);

    GameState currentGameState = GameState::MENU;
    bool whilePlaying = false;

    while (window.isOpen()) {
        //if (currentGameState == GameState::PLAYING) game.updateAI();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                // --- SWITCH STATEMENT OPTIMIZATION ---
                switch (currentGameState) {
                    case GameState::EXIT:
                        window.close();
                        break;

                    case GameState::MENU: {
                        whilePlaying = false;
                        GameState newState = mainMenu.handleEvent(event, mousePos, game);
                        if (newState == GameState::LOAD_MENU) loadScreen.refreshSlots();
                        currentGameState = newState;
                        break;
                    }

                    case GameState::NEW_GAME_MENU: {
                        std::string action = newGameScreen.handleMouseClick(mousePos);
                        if (action == "BACK") currentGameState = GameState::MENU;
                        else if (action != "") {
                            game.resetGame();
                            if (action == "PVP") {
                                game.setGameMode(GameMode::PVP);
                                //game.setDifficulty(Difficulty::NONE);
                            } else {
                                game.setGameMode(GameMode::AI);

                                Difficulty currDif = Difficulty::NONE;

                                if (action == "AI_EASY") currDif = Difficulty::EASY;
                                else if (action == "AI_MEDIUM") currDif = Difficulty::MEDIUM;
                                else if (action == "AI_HARD") currDif = Difficulty::HARD;



                                game.initAI(currDif);
                            }
                            currentGameState = GameState::PLAYING;
                        }
                        break;
                    }

                    case GameState::SETTINGS:
                        if (settingsScreen.handleClick(mousePos))
                            currentGameState = (whilePlaying ? GameState::PLAYING : GameState::MENU);
                        break;

                    case GameState::PLAYING:
                        whilePlaying = true;
                        if (mousePos.x >= BOARD_MAX_WIDTH) {
                            ui.handleButtonClick(mousePos, window, currentGameState);
                            // Prepare screens if switching
                            if (currentGameState == GameState::GAME_OVER) {
                                auto scores = game.getScore();
                                std::string msg = (scores.first > scores.second) ? "Black Wins!" : "White Wins!";
                                if (game.getGameMode() == GameMode::AI) {
                                    msg = (scores.first > scores.second) ? "YOU WIN!" : "AI WINS!";
                                }
                                std::stringstream ss;
                                ss << "B: " << scores.first << " W: " << scores.second;
                                gameOverScreen.setGameOverMessage(msg, ss.str());
                            }
                            else if (currentGameState == GameState::SAVE_MENU) saveScreen.refreshSlots();
                            else if (currentGameState == GameState::LOAD_MENU) loadScreen.refreshSlots();
                        } else {
                            handleBoardClick(game, mousePos, ui);
                        }
                        break;

                    case GameState::GAME_OVER: {
                        std::string action = gameOverScreen.handleMouseClick(mousePos);
                        if (action == "NEW") currentGameState = GameState::NEW_GAME_MENU;
                        else if (action == "LOAD") { loadScreen.refreshSlots(); currentGameState = GameState::LOAD_MENU; }
                        else if (action == "EXIT") currentGameState = GameState::MENU;
                        break;
                    }

                    case GameState::SAVE_MENU: {
                        int action = saveScreen.handleMouseClick(mousePos);
                        if (action == -2) currentGameState = (whilePlaying ? GameState::PLAYING : GameState::MENU);
                        else if (action > 0) {
                            if (game.saveGame("saves/save_0" + std::to_string(action) + ".txt"))
                                ui.setNotification("Game Saved!");
                            currentGameState = GameState::PLAYING;
                        }
                        break;
                    }

                    case GameState::LOAD_MENU: {
                        int action = loadScreen.handleMouseClick(mousePos);
                        if (action == -2) currentGameState = (whilePlaying ? GameState::PLAYING : GameState::MENU);
                        else if (action > 0) {
                            if (game.loadGame("saves/save_0" + std::to_string(action) + ".txt")) {
                                ui.setNotification("Game Loaded!");
                                currentGameState = GameState::PLAYING;
                                whilePlaying = true;
                            }
                        }
                        break;
                    }
                }
            }
        }

        window.clear();

        // --- RENDER LOGIC ---
        // Common Background Renderers
        if (currentGameState == GameState::PLAYING || currentGameState == GameState::GAME_OVER ||
            currentGameState == GameState::SAVE_MENU || currentGameState == GameState::LOAD_MENU)
        {
            GameRenderer::drawBoard(window, settings, font);
            GameRenderer::drawStones(window, game, textureManager, settings);
            ui.draw(window);
        }

        switch (currentGameState) {
            case GameState::EXIT: window.close(); break;
            case GameState::MENU: mainMenu.draw(window); break;
            case GameState::NEW_GAME_MENU: mainMenu.draw(window); newGameScreen.draw(window); break;
            case GameState::SETTINGS: settingsScreen.draw(window); break;
            case GameState::GAME_OVER: gameOverScreen.draw(window); break;
            case GameState::SAVE_MENU: saveScreen.draw(window, textureManager); break;
            case GameState::LOAD_MENU: loadScreen.draw(window, textureManager); break;
        }

        window.display();
    }
    return 0;
}