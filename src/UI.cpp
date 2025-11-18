#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <fstream> // Required for file I/O

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
const int BUTTON_START_X = WINDOW_WIDTH - PANEL_WIDTH + BUTTON_PADDING;
const int BUTTON_START_Y = GRID_OFFSET;

// Enum for stone types
enum class Stone { Empty, Black, White };

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
        // Ensure these files exist in cmake-build-debug/assets/img/
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

    // History for Undo/Redo
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
        // 1. Check validity
        if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != Stone::Empty)
            return false;

        // 2. Record the move for Undo
        Move move = {x, y, current_player};
        undoHistory.push_back(move);

        // 3. Clear Redo history (cannot redo after making a new move)
        redoHistory.clear();

        // 4. Place stone and switch turn
        board[y][x] = current_player;
        current_player = (current_player == Stone::Black ? Stone::White : Stone::Black);
        return true;
    }

    bool undo() {
        if (undoHistory.empty()) return false;

        // Get last move
        Move lastMove = undoHistory.back();
        undoHistory.pop_back();

        // Remove stone from board
        board[lastMove.y][lastMove.x] = Stone::Empty;

        // Add to Redo stack
        redoHistory.push_back(lastMove);

        // Switch turn back to the person who made that move
        current_player = lastMove.player;
        return true;
    }

    bool redo() {
        if (redoHistory.empty()) return false;

        // Get the move to redo
        Move nextMove = redoHistory.back();
        redoHistory.pop_back();

        // Place stone back on board
        board[nextMove.y][nextMove.x] = nextMove.player;

        // Add back to Undo stack
        undoHistory.push_back(nextMove);

        // Switch turn to the next player
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

        // Save current player (0 for Black, 1 for White usually, but we cast to int)
        outFile << (current_player == Stone::Black ? 1 : 2) << "\n";

        // Save Board
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

        // Clear history on load to prevent inconsistencies
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

// --- SFML UI Manager Class ---
class GoUIManager {
private:
    sf::Font font;
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
    GoUIManager(GoGame& g) : game(g) {
        if (!font.loadFromFile("assets/fonts/arial.ttf")) {
            std::cerr << "Error loading font." << std::endl;
            std::exit(1);
        }
        initializeIndicators();
        initializeButtons();
    }

    void initializeIndicators() {
        turnIndicator.setFont(font);
        turnIndicator.setCharacterSize(24);
        turnIndicator.setFillColor(sf::Color::White);
        turnIndicator.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X), static_cast<float>(BUTTON_START_Y - 40)));

        notificationText.setFont(font);
        notificationText.setCharacterSize(16);
        notificationText.setString("Ready to play.");
        notificationText.setFillColor(sf::Color::Yellow);
        notificationText.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X), static_cast<float>(WINDOW_HEIGHT - 100)));
    }

    void initializeButtons() {
        int y_pos = BUTTON_START_Y + 40;

        for (const auto& label : buttonLabels) {
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(PANEL_WIDTH - 2 * BUTTON_PADDING, BUTTON_HEIGHT));
            rect.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X), static_cast<float>(y_pos)));
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

    // Helper to update UI text
    void setNotification(const std::string& msg) {
        notificationText.setString(msg);
    }

    bool handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window) {
        for (size_t i = 0; i < mainButtonRects.size(); ++i) {
            if (mainButtonRects[i].getGlobalBounds().contains(
                    sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))))
            {
                std::string label = buttonLabels[i];

                // --- Button Logic ---
                if (label == "EXIT GAME") {
                    window.close();
                }
                else if (label == "RESET GAME" || label == "START NEW GAME") {
                    game.resetGame();
                    setNotification("Game Reset.");
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
            turnIndicator.setFillColor(sf::Color::White); // Text is white
        } else {
            turnIndicator.setString("Turn: WHITE");
            turnIndicator.setFillColor(sf::Color::Cyan); // Distinct color for white turn
        }
        window.draw(turnIndicator);

        // Draw Buttons
        for (size_t i = 0; i < mainButtonRects.size(); ++i) {
            // Highlight button on hover (optional logic could go here, keeping simple for now)
            window.draw(mainButtonRects[i]);
            window.draw(mainButtonTexts[i]);
        }

        // Draw Notification
        window.draw(notificationText);
    }
};

// --- Drawing Functions ---

void drawBoard(sf::RenderTarget& window) {
    sf::RectangleShape fullBackground(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
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

            // Scaling logic
            float scaleFactor = (STONE_RADIUS * 2.0f) / texture.getSize().x;
            stoneSprite.setScale(scaleFactor, scaleFactor);

            // Center origin
            stoneSprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);

            // Position
            stoneSprite.setPosition(toScreenCoord(x, y));

            window.draw(stoneSprite);
        }
    }
}

void handleBoardClick(GoGame& game, const sf::Vector2i& mousePos) {
    if (mousePos.x >= BOARD_MAX_WIDTH) return;

    int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
    int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
        game.placeStone(gx, gy);
    }
}

// --- Main Application Loop ---

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Go Game SFML - Complete");
    window.setFramerateLimit(60);

    GoGame game;
    GoUIManager ui(game);
    StoneTextureManager textureManager;

    sf::Event event;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);

                    if (mousePos.x >= BOARD_MAX_WIDTH) {
                        ui.handleButtonClick(mousePos, window);
                    } else {
                        handleBoardClick(game, mousePos);
                    }
                }
            }
        }

        window.clear();
        drawBoard(window);
        drawStones(window, game, textureManager);
        ui.draw(window);
        window.display();
    }

    return 0;
}