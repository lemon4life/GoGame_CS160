#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <algorithm>
// Removed: #include <optional> // Not needed for older SFML

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

// --- Forward Declarations & Helpers ---

sf::Vector2f toScreenCoord(int x, int y) {
    return sf::Vector2f(GRID_OFFSET + x * CELL_SIZE, GRID_OFFSET + y * CELL_SIZE);
}

// --- Game Logic Class ---
class GoGame {
private:
    std::vector<std::vector<Stone>> board;
    Stone current_player;

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
        board[y][x] = current_player;
        current_player = (current_player == Stone::Black ? Stone::White : Stone::Black);
        return true;
    }

    void resetGame() {
        for (auto& row : board) {
            std::fill(row.begin(), row.end(), Stone::Empty);
        }
        current_player = Stone::Black;
    }
};

// --- SFML UI Manager Class ---
class GoUIManager {
private:
    sf::Font font;
    GoGame& game;

    // Use aggregate initialization {} for members that require it (sf::Text)
    sf::Text turnIndicator{};
    sf::Text notificationText{};
    sf::Text settingsHeader{};

    std::vector<sf::RectangleShape> mainButtonRects;
    std::vector<sf::Text> mainButtonTexts;
    std::vector<std::string> buttonLabels = {
        "START NEW GAME", "RESET GAME", "UNDO MOVE", "REDO MOVE",
        "SAVE GAME", "LOAD GAME", "SETTINGS", "EXIT GAME"
    };

public:
    GoUIManager(GoGame& g)
        : game(g) // This is now the only initializer list member
    {
        // FIX 1: Change openFromFile to loadFromFile for older SFML
        if (!font.loadFromFile("assets/fonts/arial.ttf")) {
            std::cerr << "Error loading font. Make sure 'arial.ttf' is in the executable directory." << std::endl;
            std::exit(1);
        }

        initializeIndicators();
        initializeButtons();
    }

    void initializeIndicators() {
        turnIndicator.setFont(font);
        turnIndicator.setCharacterSize(24);
        turnIndicator.setFillColor(sf::Color::White);
        // Use sf::Vector2f for setPosition
        turnIndicator.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X), static_cast<float>(BUTTON_START_Y - 40)));

        notificationText.setFont(font);
        notificationText.setCharacterSize(18);
        notificationText.setString("Notifications: Game Started!");
        notificationText.setFillColor(sf::Color::Yellow);
        // Use sf::Vector2f for setPosition
        notificationText.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X), static_cast<float>(WINDOW_HEIGHT - 100)));
    }

    void initializeButtons() {
        int y_pos = BUTTON_START_Y + 40;

        for (const auto& label : buttonLabels) {
            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(PANEL_WIDTH - 2 * BUTTON_PADDING, BUTTON_HEIGHT));
            // Use sf::Vector2f for setPosition
            rect.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X), static_cast<float>(y_pos)));
            rect.setFillColor(sf::Color(100, 100, 100));
            mainButtonRects.push_back(rect);

            // FIX 2: Revert Text constructor order to (String, Font, Size) for older SFML
            sf::Text text(label, font, 16);
            text.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = text.getLocalBounds();
            // Revert to direct member access (.width, .height) as .getSize() is missing from your sf::Rect
            text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);

            // Use sf::Vector2f
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

    bool handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window) {
        for (size_t i = 0; i < mainButtonRects.size(); ++i) {

            // contains requires a sf::Vector2f
            if (mainButtonRects[i].getGlobalBounds().contains(
                    sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))))
            {

                std::string label = buttonLabels[i];
                notificationText.setString("Action: " + label);

                if (label == "EXIT GAME") {
                    window.close();
                } else if (label == "RESET GAME") {
                    game.resetGame();
                }

                return true;
            }
        }
        return false;
    }

    void draw(sf::RenderTarget& window) {
        // 1. Draw UI Panel Background
        sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, WINDOW_HEIGHT));
        // Use sf::Vector2f
        panel.setPosition(sf::Vector2f(static_cast<float>(WINDOW_WIDTH - PANEL_WIDTH), 0.0f));
        panel.setFillColor(sf::Color(40, 40, 40));
        window.draw(panel);

        // 2. Draw Indicators
        if (game.getCurrentPlayer() == Stone::Black) {
            turnIndicator.setString("Turn: BLACK");
            turnIndicator.setFillColor(sf::Color::White);
            turnIndicator.setOutlineThickness(0.0f);
        } else {
            turnIndicator.setString("Turn: WHITE");
            turnIndicator.setFillColor(sf::Color::White);
            turnIndicator.setOutlineColor(sf::Color(40, 40, 40));
            turnIndicator.setOutlineThickness(1.0f);
        }
        window.draw(turnIndicator);

        // 3. Draw Buttons
        for (size_t i = 0; i < mainButtonRects.size(); ++i) {
            window.draw(mainButtonRects[i]);
            window.draw(mainButtonTexts[i]);
        }

        // 4. Draw Notifications
        window.draw(notificationText);
    }
};

// --- Drawing Functions ---

void drawBoard(sf::RenderTarget& window) {
    sf::RectangleShape fullBackground(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    fullBackground.setFillColor(sf::Color(173, 216, 230));
    window.draw(fullBackground);

    sf::RectangleShape boardBackground(sf::Vector2f(BOARD_MAX_WIDTH, WINDOW_HEIGHT));
    boardBackground.setFillColor(sf::Color(222, 184, 135));
    window.draw(boardBackground);

    const sf::Color lineColor(50, 50, 50);
    for (int i = 0; i < BOARD_SIZE; ++i) {

        // Use sf::Vertex::Vertex(position, color) constructor
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

void drawStones(sf::RenderTarget& window, const GoGame& game) {
    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            Stone s = game.getStoneAt(x, y);
            if (s == Stone::Empty) continue;

            sf::CircleShape stone(STONE_RADIUS);
            // Use sf::Vector2f
            stone.setOrigin(sf::Vector2f(STONE_RADIUS, STONE_RADIUS));
            stone.setPosition(toScreenCoord(x, y));

            if (s == Stone::Black) {
                stone.setFillColor(sf::Color::Black);
                stone.setOutlineColor(sf::Color(80, 80, 80));
            } else {
                stone.setFillColor(sf::Color::White);
                stone.setOutlineColor(sf::Color::Black);
            }
            stone.setOutlineThickness(2.f);
            window.draw(stone);
        }
    }
}

void handleBoardClick(GoGame& game, const sf::Vector2i& mousePos) {
    if (mousePos.x >= BOARD_MAX_WIDTH) {
        return;
    }

    int gx = std::round((mousePos.x - GRID_OFFSET) / CELL_SIZE);
    int gy = std::round((mousePos.y - GRID_OFFSET) / CELL_SIZE);

    if (gx >= 0 && gx < BOARD_SIZE && gy >= 0 && gy < BOARD_SIZE) {
        game.placeStone(gx, gy);
    }
}


// --- Main Application Loop ---

int main() {
    // FIX 3: Use VideoMode(width, height) directly for older SFML
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Go Game SFML UI");
    window.setFramerateLimit(60);

    GoGame game;
    GoUIManager ui(game);

    // Event object for older SFML
    sf::Event event;

    while (window.isOpen()) {
        // FIX 4: Use classic pollEvent(event) loop
        while (window.pollEvent(event)) {

            // Check for event type using the classic structure
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                // Access mouse button data directly from event.mouseButton
                if (event.mouseButton.button == sf::Mouse::Left) {

                    // Access mouse position directly from event.mouseButton
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
        drawStones(window, game);
        ui.draw(window);
        window.display();
    }

    return 0;
}