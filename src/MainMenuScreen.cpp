#include "../include/MainMenuScreen.h"
#include <iostream>

// Re-defining constants here to ensure they match your original layout preferences
// You can move these to Definitions.h if you prefer
const float MENU_START_X = 100.0f;
const float MENU_START_Y = 280.0f;
const float MENU_BTN_WIDTH = 300.0f;
const float MENU_BTN_HEIGHT = 60.0f;
const float MENU_GAP = 20.0f;

MainMenuScreen::MainMenuScreen() {
    // 1. Load Assets
    if (!backgroundTexture.loadFromFile("assets/img/main_menu_bg.png")) {
        std::cerr << "[MainMenu] Warning: assets/img/main_menu_bg.png missing" << std::endl;
    } else {
        backgroundSprite.setTexture(backgroundTexture);
        // Scale to fit window
        float scaleX = static_cast<float>(WINDOW_WIDTH) / backgroundTexture.getSize().x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
    }

    // Load Fonts
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
         std::cerr << "[MainMenu] Error loading font" << std::endl;
    }
    // Try to load Impact for title, fallback to Arial
    if (!titleFont.loadFromFile("assets/fonts/impact.ttf")) {
        titleFont = font;
    }

    // 2. Setup Title (Restored Original Position & Style)
    titleText.setFont(titleFont);
    titleText.setString("GO GAME");
    titleText.setCharacterSize(60);
    titleText.setFillColor(sf::Color::Black);
    titleText.setOutlineColor(sf::Color::White);
    titleText.setOutlineThickness(3.0f);

    // Center title text relative to the button column on the left
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
    titleText.setPosition(MENU_START_X + MENU_BTN_WIDTH / 2.0f, 200);

    // 3. Setup Buttons (Optimized Loop)
    m_buttons.clear();

    struct MenuOption { std::string label; std::string action; };
    std::vector<MenuOption> options = {
        {"START NEW GAME", "NEW_GAME"},
        {"LOAD GAME",      "LOAD"},
        {"SETTINGS",       "SETTINGS"},
        {"EXIT",           "EXIT"}
    };

    float currentY = MENU_START_Y;

    // Grey with transparency (100, 100, 100, 200)
    sf::Color originalColor(100, 100, 100, 200);

    for (const auto& opt : options) {
        Theme::Button btn(
            font,
            opt.label,
            sf::Vector2f(MENU_BTN_WIDTH, MENU_BTN_HEIGHT),
            // Position: Left side + Half Width (because origin is center)
            sf::Vector2f(MENU_START_X + MENU_BTN_WIDTH / 2.0f, currentY + MENU_BTN_HEIGHT / 2.0f),
            originalColor,
            24 // Font Size
        );

        btn.setOutlineColor(sf::Color::White);

        m_buttons.push_back({btn, opt.action});
        currentY += MENU_BTN_HEIGHT + MENU_GAP;
    }
}

void MainMenuScreen::draw(sf::RenderWindow& window) {
    if (backgroundSprite.getTexture()) {
        window.draw(backgroundSprite);
    } else {
        window.clear(sf::Color(30, 30, 30));
    }

    window.draw(titleText);

    for (auto& pair : m_buttons) {
        pair.first.draw(window);
    }
}

GameState MainMenuScreen::handleEvent(const sf::Event& event, const sf::Vector2i& mousePos, GoGame& game) {
    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) {
        return GameState::MENU;
    }

    for (const auto& pair : m_buttons) {
        if (pair.first.isClicked(mousePos)) {
            const std::string& action = pair.second;

            if (action == "NEW_GAME") return GameState::NEW_GAME_MENU;
            if (action == "LOAD")     return GameState::LOAD_MENU;
            if (action == "SETTINGS") return GameState::SETTINGS;
            if (action == "EXIT")     return GameState::EXIT;
        }
    }

    return GameState::MENU;
}