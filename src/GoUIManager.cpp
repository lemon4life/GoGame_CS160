#include "GoUIManager.h"
#include <iostream>

GoUIManager::GoUIManager(GoGame& g) : game(g) {
    buttonLabels = {
        "START NEW GAME", "RESET GAME", "UNDO MOVE", "REDO MOVE",
        "SAVE GAME", "LOAD GAME", "SETTINGS", "EXIT GAME"
    };

    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "[GoUIManager] Error loading font" << std::endl;
    }

    initializeIndicators();
    initializeButtons();
}

// ... initializeIndicators and initializeButtons same as before ...
void GoUIManager::initializeIndicators() {
    turnIndicator.setFont(font);
    turnIndicator.setCharacterSize(24);
    turnIndicator.setFillColor(sf::Color::White);
    turnIndicator.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(BUTTON_START_Y_GAME_UI - 40)));

    notificationText.setFont(font);
    notificationText.setCharacterSize(16);
    notificationText.setString("Ready.");
    notificationText.setFillColor(sf::Color::Yellow);
    notificationText.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(WINDOW_HEIGHT - 100)));
}

void GoUIManager::initializeButtons() {
    int y_pos = BUTTON_START_Y_GAME_UI + 40;
    for (const auto& label : buttonLabels) {
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(PANEL_WIDTH - 2 * BUTTON_PADDING, BUTTON_HEIGHT));
        rect.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(y_pos)));
        rect.setFillColor(sf::Color(100, 100, 100));
        mainButtonRects.push_back(rect);

        sf::Text text(label, font, 14);
        text.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        text.setPosition(
            rect.getPosition().x + rect.getSize().x / 2.0f,
            rect.getPosition().y + rect.getSize().y / 2.0f
        );
        mainButtonTexts.push_back(text);
        y_pos += BUTTON_HEIGHT + BUTTON_PADDING;
    }
}

void GoUIManager::setNotification(const std::string& msg) {
    notificationText.setString(msg);
}

bool GoUIManager::handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window, GameState& currentGameState) {
    for (size_t i = 0; i < mainButtonRects.size(); ++i) {
        if (mainButtonRects[i].getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
            std::string label = buttonLabels[i];

            if (label == "EXIT GAME") {
                currentGameState = GameState::MENU;
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
                currentGameState = GameState::SAVE_MENU; // Switch State
            }
            else if (label == "LOAD GAME") {
                currentGameState = GameState::LOAD_MENU; // Switch State
            }
            else if (label == "SETTINGS") {
                currentGameState = GameState::SETTINGS;
            }
            else {
                setNotification("Feature not implemented.");
            }
            return true;
        }
    }
    return false;
}

void GoUIManager::draw(sf::RenderTarget& window) {
    sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, WINDOW_HEIGHT));
    panel.setPosition(sf::Vector2f(static_cast<float>(WINDOW_WIDTH - PANEL_WIDTH), 0.0f));
    panel.setFillColor(sf::Color(40, 40, 40));
    window.draw(panel);

    if (game.getCurrentPlayer() == Stone::Black) {
        turnIndicator.setString("Turn: White");
        turnIndicator.setFillColor(sf::Color::White);
    } else {
        turnIndicator.setString("Turn: Black");
        turnIndicator.setFillColor(sf::Color::Cyan);
    }
    window.draw(turnIndicator);

    for (size_t i = 0; i < mainButtonRects.size(); ++i) {
        window.draw(mainButtonRects[i]);
        window.draw(mainButtonTexts[i]);
    }
    window.draw(notificationText);
}