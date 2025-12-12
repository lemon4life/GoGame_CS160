#include "../include/GameOverScreen.h"

void GameOverScreen::init(const sf::Font& font, float width, float height) {
    background.setSize(sf::Vector2f(width, height));
    background.setFillColor(sf::Color(0, 0, 0, 150));

    // Main Box
    float boxW = 500;
    float boxH = 400;
    container.setSize(sf::Vector2f(boxW, boxH));
    container.setOrigin(boxW / 2.0f, boxH / 2.0f);
    container.setPosition(width / 2.0f, height / 2.0f);
    container.setFillColor(Theme::Background);
    container.setOutlineThickness(3);
    container.setOutlineColor(sf::Color::White);

    // Texts
    titleText.setFont(font);
    titleText.setCharacterSize(40);
    titleText.setFillColor(Theme::TextNormal);

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);

    // Clear previous buttons
    m_buttons.clear();

    // Define Buttons Configuration
    struct BtnConfig { std::string label; std::string action; sf::Color color; };
    std::vector<BtnConfig> configs = {
        {"NEW GAME",  "NEW",  Theme::BtnSuccess}, // Green
        {"LOAD GAME", "LOAD", Theme::BtnPrimary}, // Blue
        {"EXIT",      "EXIT", Theme::BtnDanger}   // Red
    };

    float startY = height / 2.0f - 20;

    for (const auto& cfg : configs) {
        m_buttons.push_back({
            Theme::Button(
                font, cfg.label,
                sf::Vector2f(250, 50),
                sf::Vector2f(width / 2.0f, startY),
                cfg.color
            ),
            cfg.action
        });
        startY += 70;
    }
}

void GameOverScreen::setGameOverMessage(const std::string& message, const std::string& scoreDetail) {
    // Center Title
    titleText.setString(message);
    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    titleText.setPosition(container.getPosition().x, container.getPosition().y - 150);

    // Center Score
    scoreText.setString(scoreDetail);
    sf::FloatRect sb = scoreText.getLocalBounds();
    scoreText.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
    scoreText.setPosition(container.getPosition().x, container.getPosition().y - 80);
}

void GameOverScreen::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(container);
    window.draw(titleText);
    window.draw(scoreText);

    for (auto& pair : m_buttons) {
        pair.first.draw(window);
    }
}

std::string GameOverScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    for (const auto& pair : m_buttons) {
        if (pair.first.isClicked(mousePos)) {
            return pair.second;
        }
    }
    return "";
}