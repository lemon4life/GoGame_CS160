#include "NewGameScreen.h"

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

    m_buttons.clear();

    // Define data clearly
    struct BtnData { std::string label; std::string action; sf::Color color; };
    std::vector<BtnData> data = {
        {"2-PLAYER MODE",   "PVP",       sf::Color(100, 100, 200)},
        {"VS AI (EASY)",    "AI_EASY",   sf::Color(50, 150, 50)},
        {"VS AI (MEDIUM)",  "AI_MEDIUM", sf::Color(200, 150, 50)},
        {"VS AI (HARD)",    "AI_HARD",   sf::Color(200, 50, 50)},
        {"BACK",            "BACK",      sf::Color(100, 100, 100)}
    };

    float startY = height / 2.0f - 100;

    // Create Buttons in one clean loop
    for (const auto& d : data) {
        m_buttons.push_back({
            Theme::Button(font, d.label, {300.f, 50.f}, {width / 2.0f, startY}, d.color),
            d.action
        });
        startY += 65;
    }
}

void NewGameScreen::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(container);
    window.draw(titleText);

    // Clean drawing loop
    for (auto& pair : m_buttons) {
        pair.first.draw(window);
    }
}

std::string NewGameScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    for (auto& pair : m_buttons) {
        if (pair.first.isClicked(mousePos)) {
            return pair.second;
        }
    }
    return "";
}