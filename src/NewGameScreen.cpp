#include "../include/NewGameScreen.h"

void NewGameScreen::init(const sf::Font& font, float width, float height) {
    // Semi-transparent background
    background.setSize(sf::Vector2f(width, height));
    background.setFillColor(sf::Color(0, 0, 0, 200));

    // Center Container
    float boxW = 400;
    float boxH = 450;
    container.setSize(sf::Vector2f(boxW, boxH));
    container.setOrigin(boxW / 2.0f, boxH / 2.0f);
    container.setPosition(width / 2.0f, height / 2.0f);
    container.setFillColor(sf::Color(50, 50, 50));
    container.setOutlineThickness(2);
    container.setOutlineColor(sf::Color::White);

    // Title
    titleText.setFont(font);
    titleText.setString("SELECT MODE");
    titleText.setCharacterSize(36);
    titleText.setFillColor(sf::Color::White);
    sf::FloatRect tr = titleText.getLocalBounds();
    titleText.setOrigin(tr.width / 2.0f, tr.height / 2.0f);
    titleText.setPosition(width / 2.0f, height / 2.0f - 180);

    // Buttons
    struct BtnConfig { std::string label; std::string action; sf::Color color; };
    std::vector<BtnConfig> configs = {
        {"2-PLAYER MODE", "PVP", sf::Color(100, 100, 200)}, // Blue
        {"VS AI (EASY)", "AI_EASY", sf::Color(50, 150, 50)},   // Green
        {"VS AI (MEDIUM)", "AI_MEDIUM", sf::Color(200, 150, 50)}, // Orange
        {"VS AI (HARD)", "AI_HARD", sf::Color(200, 50, 50)},    // Red
        {"BACK", "BACK", sf::Color(100, 100, 100)}              // Grey
    };

    float startY = height / 2.0f - 100;
    for (const auto& cfg : configs) {
        OptionButton btn;
        btn.action = cfg.action;
        
        btn.rect.setSize(sf::Vector2f(300, 50));
        btn.rect.setOrigin(150, 25);
        btn.rect.setPosition(width / 2.0f, startY);
        btn.rect.setFillColor(cfg.color);
        btn.rect.setOutlineThickness(1);
        btn.rect.setOutlineColor(sf::Color::White);

        btn.text.setFont(font);
        btn.text.setString(cfg.label);
        btn.text.setCharacterSize(18);
        btn.text.setFillColor(sf::Color::White);
        sf::FloatRect br = btn.text.getLocalBounds();
        btn.text.setOrigin(br.width / 2.0f, br.height / 2.0f);
        btn.text.setPosition(btn.rect.getPosition());

        buttons.push_back(btn);
        startY += 65;
    }
}

void NewGameScreen::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(container);
    window.draw(titleText);
    for (const auto& btn : buttons) {
        window.draw(btn.rect);
        window.draw(btn.text);
    }
}

std::string NewGameScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    for (const auto& btn : buttons) {
        if (btn.rect.getGlobalBounds().contains(mPos)) {
            return btn.action;
        }
    }
    return "";
}