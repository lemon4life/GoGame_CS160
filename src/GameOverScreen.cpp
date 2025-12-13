#include "GameOverScreen.h"

void GameOverScreen::init(const sf::Font& font, float width, float height) {
    // Dark Overlay
    background.setSize(sf::Vector2f(width, height));
    background.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent

    // Main Box
    float boxW = 500;
    float boxH = 400;
    container.setSize(sf::Vector2f(boxW, boxH));
    container.setOrigin(boxW / 2.0f, boxH / 2.0f);
    container.setPosition(width / 2.0f, height / 2.0f);
    container.setFillColor(sf::Color(50, 50, 50));
    container.setOutlineThickness(3);
    container.setOutlineColor(sf::Color::White);

    // Texts
    titleText.setFont(font);
    titleText.setCharacterSize(40);
    titleText.setFillColor(sf::Color::White);
    
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);

    // Buttons: NEW GAME, LOAD GAME, EXIT
    std::vector<std::string> labels = {"NEW GAME", "LOAD GAME", "EXIT"};
    std::vector<std::string> actions = {"NEW", "LOAD", "EXIT"};
    
    float startY = height / 2.0f + 20; 
    
    for (size_t i = 0; i < labels.size(); ++i) {
        Button btn;
        btn.action = actions[i];
        
        btn.rect.setSize(sf::Vector2f(250, 50));
        btn.rect.setOrigin(125, 25);
        btn.rect.setPosition(width / 2.0f, startY + (i * 70));
        btn.rect.setFillColor(sf::Color(80, 80, 80));
        btn.rect.setOutlineThickness(1);
        btn.rect.setOutlineColor(sf::Color::White);

        btn.text.setFont(font);
        btn.text.setString(labels[i]);
        btn.text.setCharacterSize(20);
        btn.text.setFillColor(sf::Color::White);
        sf::FloatRect b = btn.text.getLocalBounds();
        btn.text.setOrigin(b.width / 2.0f, b.height / 2.0f);
        btn.text.setPosition(btn.rect.getPosition());

        buttons.push_back(btn);
    }
}

void GameOverScreen::setGameOverMessage(const std::string& message, const std::string& scoreDetail) {
    titleText.setString(message);
    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin(tb.width / 2.0f, tb.height / 2.0f);
    // Position relative to container center (screen center)
    // Container is at center. Top of container is center.y - 200
    titleText.setPosition(container.getPosition().x, container.getPosition().y - 150);

    scoreText.setString(scoreDetail);
    sf::FloatRect sb = scoreText.getLocalBounds();
    scoreText.setOrigin(sb.width / 2.0f, sb.height / 2.0f);
    scoreText.setPosition(container.getPosition().x, container.getPosition().y - 80);
}

void GameOverScreen::draw(sf::RenderWindow& window) {
    window.draw(background);
    window.draw(container);
    window.draw(titleText);
    window.draw(scoreText);
    
    for (const auto& btn : buttons) {
        window.draw(btn.rect);
        window.draw(btn.text);
    }
}

std::string GameOverScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mPos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    
    for (const auto& btn : buttons) {
        if (btn.rect.getGlobalBounds().contains(mPos)) {
            return btn.action;
        }
    }
    return "";
}