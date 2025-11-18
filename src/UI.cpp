#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream>

// --- Constants ---
const int BOARD_SIZE = 9;
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 850;
const int GRID_OFFSET = 50;
const float STONE_RADIUS = 20.0f;
const int PANEL_WIDTH = 200;
const float BOARD_MAX_WIDTH = WINDOW_WIDTH - PANEL_WIDTH;
const float CELL_SIZE = (WINDOW_HEIGHT - 2 * GRID_OFFSET) / (BOARD_SIZE - 1.0f);

// --- UI Constants ---
const int BUTTON_HEIGHT = 40;
const int BUTTON_PADDING = 10;
const int BUTTON_START_X_GAME_UI = WINDOW_WIDTH - PANEL_WIDTH + BUTTON_PADDING; // Specific for in-game UI
const int BUTTON_START_Y_GAME_UI = GRID_OFFSET;

// --- Main Menu Specific Constants ---
const int MENU_BUTTON_WIDTH = 250;
const int MENU_BUTTON_HEIGHT = 50;
const int MENU_BUTTON_START_X = 100; // Left side for menu buttons
const int MENU_BUTTON_START_Y = 250; // Starting Y position for first menu button
const int MENU_TITLE_Y = 80;


// Enum for stone types
enum class Stone { Empty, Black, White };

// Enum for overall game state
enum class GameState { MENU, PLAYING, SETTINGS, EXIT };

// Struct to record a move for Undo/Redo
struct Move {
    int x;
    int y;
    Stone player; // Who placed this stone
};

// --- Forward Declarations & Helpers ---

sf::Vector2f toScreenCoord(int x, int y) {
    return sf::Vector2f(GRID_OFFSET + x * CELL_SIZE, GRID_OFFSET + y * CELL_SIZE);
}

// --- Texture Management Class ---
class StoneTextureManager {
private:
    sf::Texture blackStoneTexture;
    sf::Texture whiteStoneTexture;

public:
    StoneTextureManager() {
        if (!blackStoneTexture.loadFromFile("assets/img/blackstone.png")) {
            std::cerr << "Error loading black stone texture." << std::endl;
            std::exit(1);
        }
        if (!whiteStoneTexture.loadFromFile("assets/img/whitestone.png")) {
            std::cerr << "Error loading white stone texture." << std::endl;
            std::exit(1);
        }
    }

    const sf::Texture& getTexture(Stone type) const {
        if (type == Stone::Black) return blackStoneTexture;
        return whiteStoneTexture;
    }
};

// --- Game Logic Class ---
class GoGame {
private:
    std::vector<std::vector<Stone>> board;
    Stone current_player;

    std::vector<Move> undoHistory;
    std::vector<Move> redoHistory;

public:
    GoGame() : current_player(Stone::Black) {
        board.resize(BOARD_SIZE, std::vector<Stone>(BOARD_SIZE, Stone::Empty));
    }

    Stone getCurrentPlayer() const { return current_player; }

    Stone getStoneAt(int x, int y) const {
        if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
            return Stone::Empty;
        return board[y][x];
    }

    bool placeStone(int x, int y) {
        if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != Stone::Empty)
            return false;

        Move move = {x, y, current_player};
        undoHistory.push_back(move);
        redoHistory.clear();

        board[y][x] = current_player;
        current_player = (current_player == Stone::Black ? Stone::White : Stone::Black);
        return true;
    }

    bool undo() {
        if (undoHistory.empty()) return false;

        Move lastMove = undoHistory.back();
        undoHistory.pop_back();

        board[lastMove.y][lastMove.x] = Stone::Empty;
        redoHistory.push_back(lastMove);
        current_player = lastMove.player;
        return true;
    }

    bool redo() {
        if (redoHistory.empty()) return false;

        Move nextMove = redoHistory.back();
        redoHistory.pop_back();

        board[nextMove.y][nextMove.x] = nextMove.player;
        undoHistory.push_back(nextMove);
        current_player = (nextMove.player == Stone::Black ? Stone::White : Stone::Black);
        return true;
    }

    void resetGame() {
        for (auto& row : board) {
            std::fill(row.begin(), row.end(), Stone::Empty);
        }
        current_player = Stone::Black;
        undoHistory.clear();
        redoHistory.clear();
    }

    bool saveGame(const std::string& filename) {
        std::ofstream outFile(filename);
        if (!outFile.is_open()) return false;

        outFile << (current_player == Stone::Black ? 1 : 2) << "\n"; // Save current player

        for (const auto& row : board) {
            for (const auto& cell : row) {
                int val = 0;
                if (cell == Stone::Black) val = 1;
                if (cell == Stone::White) val = 2;
                outFile << val << " ";
            }
            outFile << "\n";
        }
        return true;
    }

    bool loadGame(const std::string& filename) {
        std::ifstream inFile(filename);
        if (!inFile.is_open()) return false;

        int playerVal;
        inFile >> playerVal;
        current_player = (playerVal == 1 ? Stone::Black : Stone::White);

        undoHistory.clear();
        redoHistory.clear();

        for (int y = 0; y < BOARD_SIZE; ++y) {
            for (int x = 0; x < BOARD_SIZE; ++x) {
                int val;
                inFile >> val;
                if (val == 1) board[y][x] = Stone::Black;
                else if (val == 2) board[y][x] = Stone::White;
                else board[y][x] = Stone::Empty;
            }
        }
        return true;
    }
};

// --- In-Game UI Manager Class (Renamed and slightly adjusted constants) ---
class GoUIManager {
private:

    GoGame& game;

    sf::Text turnIndicator{};
    sf::Text notificationText{};

    std::vector<sf::RectangleShape> mainButtonRects;
    std::vector<sf::Text> mainButtonTexts;
    std::vector<std::string> buttonLabels = {
        "START NEW GAME", "RESET GAME", "UNDO MOVE", "REDO MOVE",
        "SAVE GAME", "LOAD GAME", "SETTINGS", "EXIT GAME"
    };

public:
    sf::Font font;

    GoUIManager(GoGame& g) : game(g) {
        if (!font.loadFromFile("assets/fonts/arial.ttf")) {
            std::cerr << "Error loading font for GoUIManager." << std::endl;
            // Don't exit here, main menu might still work with default font or different assets
            // std::exit(1);
        }
        initializeIndicators();
        initializeButtons();
    }

    void initializeIndicators() {
        turnIndicator.setFont(font);
        turnIndicator.setCharacterSize(24);
        turnIndicator.setFillColor(sf::Color::White);
        turnIndicator.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(BUTTON_START_Y_GAME_UI - 40)));

        notificationText.setFont(font);
        notificationText.setCharacterSize(16);
        notificationText.setString("Ready to play.");
        notificationText.setFillColor(sf::Color::Yellow);
        notificationText.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(WINDOW_HEIGHT - 100)));
    }

    void initializeButtons() {
        int y_pos = BUTTON_START_Y_GAME_UI + 40;

        for (const auto& label : buttonLabels) {
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(PANEL_WIDTH - 2 * BUTTON_PADDING, BUTTON_HEIGHT));
            rect.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(y_pos)));
            rect.setFillColor(sf::Color(100, 100, 100));
            mainButtonRects.push_back(rect);

            sf::Text text(label, font, 14); // Slightly smaller font to fit
            text.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);

            text.setPosition(
                sf::Vector2f(
                    rect.getPosition().x + rect.getSize().x / 2.0f,
                    rect.getPosition().y + rect.getSize().y / 2.0f
                )
            );

            mainButtonTexts.push_back(text);
            y_pos += BUTTON_HEIGHT + BUTTON_PADDING;
        }
    }

    void setNotification(const std::string& msg) {
        notificationText.setString(msg);
    }

    bool handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window, GameState& currentGameState) {
        for (size_t i = 0; i < mainButtonRects.size(); ++i) {
            if (mainButtonRects[i].getGlobalBounds().contains(
                    sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))))
            {
                std::string label = buttonLabels[i];

                if (label == "EXIT GAME") {
                    currentGameState = GameState::EXIT; // Signal main loop to exit
                }
                else if (label == "RESET GAME" || label == "START NEW GAME") {
                    game.resetGame();
                    setNotification("New Game Started.");
                }
                else if (label == "UNDO MOVE") {
                    if (game.undo()) setNotification("Move Undone.");
                    else setNotification("Cannot Undo.");
                }
                else if (label == "REDO MOVE") {
                    if (game.redo()) setNotification("Move Redone.");
                    else setNotification("Cannot Redo.");
                }
                else if (label == "SAVE GAME") {
                    if (game.saveGame("savegame.txt")) setNotification("Game Saved!");
                    else setNotification("Save Failed.");
                }
                else if (label == "LOAD GAME") {
                    if (game.loadGame("savegame.txt")) setNotification("Game Loaded!");
                    else setNotification("Load Failed (No file?).");
                }
                else if (label == "SETTINGS") {
                    currentGameState = GameState::SETTINGS;
                    setNotification("Entering Settings.");
                }
                else {
                    setNotification("Feature not implemented.");
                }
                return true;
            }
        }
        return false;
    }

    void draw(sf::RenderTarget& window) {
        // Draw Panel
        sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, WINDOW_HEIGHT));
        panel.setPosition(sf::Vector2f(static_cast<float>(WINDOW_WIDTH - PANEL_WIDTH), 0.0f));
        panel.setFillColor(sf::Color(40, 40, 40));
        window.draw(panel);

        // Draw Indicator
        if (game.getCurrentPlayer() == Stone::Black) {
            turnIndicator.setString("Turn: BLACK");
            turnIndicator.setFillColor(sf::Color::White);
        } else {
            turnIndicator.setString("Turn: WHITE");
            turnIndicator.setFillColor(sf::Color::Cyan);
        }
        window.draw(turnIndicator);

        // Draw Buttons
        for (size_t i = 0; i < mainButtonRects.size(); ++i) {
            window.draw(mainButtonRects[i]);
            window.draw(mainButtonTexts[i]);
        }

        // Draw Notification
        window.draw(notificationText);
    }
};

// --- Main Menu Screen Class ---
class MainMenuScreen {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Font titleFont;
    sf::Font buttonFont;
    sf::Text titleText;

    std::vector<sf::RectangleShape> menuButtonRects;
    std::vector<sf::Text> menuButtonTexts;
    std::vector<std::string> buttonLabels = {
        "NEW GAME", "LOAD GAME", "SETTINGS", "EXIT"
    };

public:
    MainMenuScreen() {
        // Load background image
        if (!backgroundTexture.loadFromFile("assets/img/main_menu_bg.png")) {
            std::cerr << "Error loading main menu background: assets/img/main_menu_bg.png" << std::endl;
            // If background fails, try to continue without it.
        } else {
            backgroundSprite.setTexture(backgroundTexture);
            // Scale background to window size, maintaining aspect ratio
            float scaleX = static_cast<float>(WINDOW_WIDTH) / backgroundTexture.getSize().x;
            float scaleY = static_cast<float>(WINDOW_HEIGHT) / backgroundTexture.getSize().y;
            backgroundSprite.setScale(scaleX, scaleY);
        }

        // Load fonts
        if (!titleFont.loadFromFile("assets/fonts/Minecrafter.ttf")) { // Custom font for title
            std::cerr << "Error loading title font. Using Arial fallback." << std::endl;
            if (!titleFont.loadFromFile("assets/fonts/arial.ttf")) { // Fallback
                 std::cerr << "Error loading Arial fallback font. Main menu title may be missing." << std::endl;
            }
        }
        if (!buttonFont.loadFromFile("assets/fonts/arial.ttf")) { // Arial for buttons
            std::cerr << "Error loading button font. Buttons may be missing." << std::endl;
        }

        // Initialize Title
        titleText.setFont(titleFont);
        titleText.setString("GO GAME"); // Title of your game
        titleText.setCharacterSize(72);
        titleText.setFillColor(sf::Color::Black); // Example color
        titleText.setOutlineColor(sf::Color::White);
        titleText.setOutlineThickness(3.0f);

        // Center the title text (assuming it's on the left, not literally centered on window)
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
        titleText.setPosition(MENU_BUTTON_START_X + MENU_BUTTON_WIDTH / 2.0f, MENU_TITLE_Y + titleBounds.height / 2.0f);


        // Initialize Buttons
        int y_pos = MENU_BUTTON_START_Y;
        for (const auto& label : buttonLabels) {
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT));
            rect.setPosition(static_cast<float>(MENU_BUTTON_START_X), static_cast<float>(y_pos));
            rect.setFillColor(sf::Color(100, 100, 100, 200)); // Semi-transparent grey
            rect.setOutlineThickness(2.0f);
            rect.setOutlineColor(sf::Color::White);
            menuButtonRects.push_back(rect);

            sf::Text text(label, buttonFont, 24);
            text.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
            text.setPosition(
                rect.getPosition().x + rect.getSize().x / 2.0f,
                rect.getPosition().y + rect.getSize().y / 2.0f
            );
            menuButtonTexts.push_back(text);
            y_pos += MENU_BUTTON_HEIGHT + BUTTON_PADDING;
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(backgroundSprite); // Draw background first
        window.draw(titleText);

        for (size_t i = 0; i < menuButtonRects.size(); ++i) {
            window.draw(menuButtonRects[i]);
            window.draw(menuButtonTexts[i]);
        }
    }

    GameState handleEvent(const sf::Event& event, const sf::Vector2i& mousePos, GoGame& game) {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            for (size_t i = 0; i < menuButtonRects.size(); ++i) {
                if (menuButtonRects[i].getGlobalBounds().contains(
                        static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                {
                    std::string label = buttonLabels[i];
                    if (label == "NEW GAME") {
                        game.resetGame(); // Ensure game is fresh
                        return GameState::PLAYING;
                    } else if (label == "LOAD GAME") {
                        if (game.loadGame("savegame.txt")) {
                            std::cout << "Game loaded from savegame.txt" << std::endl; // For debugging
                            // You might want a notification in the main game UI for this
                            return GameState::PLAYING;
                        } else {
                            std::cerr << "Failed to load game." << std::endl;
                            // Maybe add a temporary error message on the menu screen
                            return GameState::MENU; // Stay on menu if load fails
                        }
                    } else if (label == "SETTINGS") {
                        return GameState::SETTINGS;
                    } else if (label == "EXIT") {
                        return GameState::EXIT;
                    }
                }
            }
        }
        return GameState::MENU; // Default to staying in menu
    }
};


// --- Drawing Functions (Unchanged) ---

void drawBoard(sf::RenderTarget& window) {
    sf::RectangleShape fullBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT)); // Only cover board area
    fullBackground.setFillColor(sf::Color(60, 60, 60)); // Darker background for app
    window.draw(fullBackground);

    sf::RectangleShape boardBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
    boardBackground.setFillColor(sf::Color(222, 184, 135)); // Wood color
    window.draw(boardBackground);

    const sf::Color lineColor(0, 0, 0);
    for (int i = 0; i < BOARD_SIZE; ++i) {
        sf::Vertex line1[] = {
            sf::Vertex(toScreenCoord(i, 0), lineColor),
            sf::Vertex(toScreenCoord(i, BOARD_SIZE - 1), lineColor)
        };
        window.draw(line1, 2, sf::PrimitiveType::Lines);

        sf::Vertex line2[] = {
            sf::Vertex(toScreenCoord(0, i), lineColor),
            sf::Vertex(toScreenCoord(BOARD_SIZE - 1, i), lineColor)
        };
        window.draw(line2, 2, sf::PrimitiveType::Lines);
    }
}

void drawStones(sf::RenderTarget& window, const GoGame& game, const StoneTextureManager& textureManager) {
    sf::Sprite stoneSprite;

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            Stone s = game.getStoneAt(x, y);
            if (s == Stone::Empty) continue;

            const sf::Texture& texture = textureManager.getTexture(s);
            stoneSprite.setTexture(texture);

            float scaleFactor = (STONE_RADIUS * 2.0f) / texture.getSize().x;
            stoneSprite.setScale(scaleFactor, scaleFactor);

            stoneSprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

            stoneSprite.setPosition(toScreenCoord(x, y));

            window.draw(stoneSprite);
        }
    }
}

void handleBoardClick(GoGame& game, const sf::Vector2i& mousePos, GoUIManager& ui) {
    if (mousePos.x >= BOARD_MAX_WIDTH) return;

    int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
    int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
        if (game.placeStone(gx, gy)) {
            ui.setNotification("Stone placed at (" + std::to_string(gx) + "," + std::to_string(gy) + ")");
        } else {
            ui.setNotification("Invalid move.");
        }
    }
}

// --- Main Application Loop ---

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Go Game SFML - Complete");
    window.setFramerateLimit(60);

    GoGame game;
    GoUIManager ui(game); // In-game UI
    StoneTextureManager textureManager;
    MainMenuScreen mainMenu; // NEW: Main Menu

    GameState currentGameState = GameState::MENU; // Start at the menu
    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Handle mouse clicks based on current game state
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                if (currentGameState == GameState::MENU) {
                    currentGameState = mainMenu.handleEvent(event, mousePos, game);
                } else if (currentGameState == GameState::PLAYING) {
                    // Check if click is on UI panel or board
                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        ui.handleButtonClick(mousePos, window, currentGameState); // Pass game state to allow exit/settings
                    } else {
                        handleBoardClick(game, mousePos, ui);
                    }
                } else if (currentGameState == GameState::SETTINGS) {
                    // Placeholder: For now, clicking anywhere in settings takes you back to menu
                    // In a real game, you'd have buttons for "Apply", "Back", etc.
                    currentGameState = GameState::MENU;
                }
            }
        }

        // Handle game state transitions
        if (currentGameState == GameState::EXIT) {
            window.close();
        }

        window.clear();

        // Draw based on current game state
        if (currentGameState == GameState::MENU) {
            mainMenu.draw(window);
        } else if (currentGameState == GameState::PLAYING) {
            drawBoard(window);
            drawStones(window, game, textureManager);
            ui.draw(window); // Draw in-game UI panel
        } else if (currentGameState == GameState::SETTINGS) {
            // Placeholder: Simple settings screen
            sf::Text settingsText("SETTINGS SCREEN\n(Click to return to Main Menu)", ui.font, 30);
            settingsText.setFillColor(sf::Color::White);
            settingsText.setPosition(WINDOW_WIDTH / 2.0f - settingsText.getLocalBounds().width / 2.0f,
                                     WINDOW_HEIGHT / 2.0f - settingsText.getLocalBounds().height / 2.0f);
            window.draw(settingsText);
        }

        window.display();
    }

    return 0;
}