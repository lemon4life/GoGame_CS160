#include "MainMenuScreen.h"
#include <iostream>

MainMenuScreen::MainMenuScreen() {
    buttonLabels = { "NEW GAME", "LOAD GAME", "SETTINGS", "EXIT" };

    // Load Background
    if (!backgroundTexture.loadFromFile("assets/img/main_menu_bg.png")) {
        std::cerr << "[MainMenu] Warning: assets/img/main_menu_bg.png missing" << std::endl;
    } else {
        backgroundSprite.setTexture(backgroundTexture);
        float scaleX = static_cast<float>(WINDOW_WIDTH) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    // Load Fonts
    if (!titleFont.loadFromFile("assets/fonts/impact.ttf")) {
        if (!titleFont.loadFromFile("assets/fonts/arial.ttf")) {
            std::cerr << "[MainMenu] Error: Could not load any title font." << std::endl;
        }
    }
    if (!buttonFont.loadFromFile("assets/fonts/arial.ttf")) {
         std::cerr << "[MainMenu] Error: Could not load button font." << std::endl;
    }

    // Setup Title
    titleText.setFont(titleFont);
    titleText.setString("GO GAME");
    titleText.setCharacterSize(72);
    titleText.setFillColor(sf::Color::Black);
    titleText.setOutlineColor(sf::Color::White);
    titleText.setOutlineThickness(3.0f);
    
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    titleText.setPosition(MENU_START_X + MENU_BTN_WIDTH / 2.0f, 150);

    // Setup Buttons
    int y_pos = MENU_START_Y;
    for (const auto& label : buttonLabels) {
        sf::RectangleShape rect;
        rect.setSize(sf::Vector2f(MENU_BTN_WIDTH, MENU_BTN_HEIGHT));
        rect.setPosition(static_cast<float>(MENU_START_X), static_cast<float>(y_pos));
        rect.setFillColor(sf::Color(100, 100, 100, 200));
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
        y_pos += MENU_BTN_HEIGHT + BUTTON_PADDING;
    }
}

void MainMenuScreen::draw(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    window.draw(titleText);
    for (size_t i = 0; i < menuButtonRects.size(); ++i) {
        window.draw(menuButtonRects[i]);
        window.draw(menuButtonTexts[i]);
    }
}

GameState MainMenuScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos, GoGame& game) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        for (size_t i = 0; i < menuButtonRects.size(); ++i) {
            if (menuButtonRects[i].getGlobalBounds().contains(
                    static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
            {
                std::string label = buttonLabels[i];

                if (label == "NEW GAME") {
                    game.resetGame();
                    return GameState::PLAYING;
                }
                else if (label == "LOAD GAME") {
                    // CHANGE: Return LOAD_MENU state instead of loading immediately
                    return GameState::LOAD_MENU;
                } 
                else if (label == "SETTINGS") {
                    return GameState::SETTINGS;
                } 
                else if (label == "EXIT") {
                    return GameState::EXIT;
                }
            }
        }
    }
    return GameState::MENU;
}