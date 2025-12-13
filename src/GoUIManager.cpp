#include "../include/GoUIManager.h"
#include <iostream>

GoUIManager::GoUIManager(GoGame& g) : game(g) {
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "[GoUIManager] Error loading font" << std::endl;
    }

    initializeIndicators();
    initializeButtons();
}

void GoUIManager::initializeIndicators() {
    turnIndicator.setFont(font);
    turnIndicator.setCharacterSize(24);
    turnIndicator.setFillColor(Theme::TextNormal);
    turnIndicator.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(BUTTON_START_Y_GAME_UI - 40)));

    notificationText.setFont(font);
    notificationText.setCharacterSize(16);
    notificationText.setString("Ready.");
    notificationText.setFillColor(sf::Color::Yellow);
    notificationText.setPosition(sf::Vector2f(static_cast<float>(BUTTON_START_X_GAME_UI), static_cast<float>(WINDOW_HEIGHT - 100)));
}

void GoUIManager::initializeButtons() {
    // 1. Initialize Pass Button
    passButton = std::make_unique<Theme::Button>(
        font, "PASS MOVE",
        sf::Vector2f(PANEL_WIDTH - 2 * BUTTON_PADDING, BUTTON_HEIGHT),
        sf::Vector2f(BUTTON_START_X_GAME_UI + (PANEL_WIDTH - 2 * BUTTON_PADDING)/2.0f, BUTTON_START_Y_GAME_UI + BUTTON_HEIGHT/2.0f),
        Theme::BtnPrimary // Blue
    );

    // 2. Initialize Main Buttons
    std::vector<std::string> labels = {
        "START NEW GAME", "RESET GAME", "UNDO MOVE", "REDO MOVE",
        "SAVE GAME", "LOAD GAME", "SETTINGS", "EXIT GAME"
    };

    float currentY = BUTTON_START_Y_GAME_UI + 60;

    for (const auto& label : labels) {
        auto btn = std::make_unique<Theme::Button>(
            font, label,
            sf::Vector2f(PANEL_WIDTH - 2 * BUTTON_PADDING, BUTTON_HEIGHT),
            sf::Vector2f(BUTTON_START_X_GAME_UI + (PANEL_WIDTH - 2 * BUTTON_PADDING)/2.0f, currentY + BUTTON_HEIGHT/2.0f),
            Theme::BtnDisabled, // Dark Grey
            14 // Smaller font for list
        );

        m_buttons.push_back({std::move(btn), label});
        currentY += BUTTON_HEIGHT + BUTTON_PADDING;
    }
}

void GoUIManager::setNotification(const std::string& msg) {
    notificationText.setString(msg);
}

bool GoUIManager::handleButtonClick(const sf::Vector2i& mousePos, sf::RenderWindow& window, GameState& currentGameState) {
    // 1. Check Pass Button
    if (passButton->isClicked(mousePos)) {
        bool gameOver = game.passTurn();
        if (gameOver) {
            currentGameState = GameState::GAME_OVER;
        } else {
            setNotification("Player Passed.");
        }
        return true;
    }

    // 2. Check Main Buttons
    for (const auto& pair : m_buttons) {
        if (pair.first->isClicked(mousePos)) {
            const std::string& label = pair.second;

            if (label == "EXIT GAME") {
                currentGameState = GameState::MENU;
            }
            else if (label == "RESET GAME" || label == "START NEW GAME") {
                // Usually triggers the New Game Menu now, but depends on your Main.cpp logic
                // If you want it to go to the mode selection:
                 currentGameState = GameState::NEW_GAME_MENU;
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
                currentGameState = GameState::SAVE_MENU;
            }
            else if (label == "LOAD GAME") {
                currentGameState = GameState::LOAD_MENU;
            }
            else if (label == "SETTINGS") {
                currentGameState = GameState::SETTINGS;
            }

            return true;
        }
    }
    return false;
}

void GoUIManager::draw(sf::RenderWindow& window) {
    // Draw Panel Background
    sf::RectangleShape panel(sf::Vector2f(PANEL_WIDTH, WINDOW_HEIGHT));
    panel.setPosition(sf::Vector2f(static_cast<float>(WINDOW_WIDTH - PANEL_WIDTH), 0.0f));
    panel.setFillColor(sf::Color(40, 40, 40));
    window.draw(panel);

    // Update Turn Indicator
    if (game.getCurrentPlayer() == Stone::Black) {
        turnIndicator.setString("Turn: Black");
        turnIndicator.setFillColor(sf::Color::White);
    } else {
        if (game.isAIThinking()) {
            turnIndicator.setString("AI Thinking...");
            turnIndicator.setFillColor(sf::Color::Red);
        } else {
            turnIndicator.setString("Turn: White");
            turnIndicator.setFillColor(sf::Color::Cyan);
        }
    }

    window.draw(turnIndicator);

    // Draw Buttons
    passButton->draw(window);
    for (const auto& pair : m_buttons) {
        pair.first->draw(window);
    }

    window.draw(notificationText);
}