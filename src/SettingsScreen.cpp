#include "SettingsScreen.h"
#include <iostream>
#include <algorithm>

SettingsScreen::SettingsScreen(GameSettings& s, StoneTextureManager& tm, AudioManager& am) 
    : settings(s), textureMgr(tm), audioMgr(am) 
{
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "[SettingsScreen] Error loading assets/fonts/arial.ttf" << std::endl;
    }

    // Setup Title
    titleText.setFont(font);
    titleText.setString("SETTINGS");
    titleText.setCharacterSize(40);
    titleText.setPosition(WINDOW_WIDTH / 2.0f - 100, 50);
    titleText.setFillColor(sf::Color::White);

    // Setup Dark Overlay Background
    background.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(sf::Color(30, 30, 30));

    // Create the main setting buttons
    createButton(0, "Board Theme: ", 150);
    createButton(1, "Stone Style: ", 220);
    createButton(2, "Sound Effects: ", 290);
    createButton(3, "Music: ", 360);
    
    // Volume Control Buttons
    createButton(4, "Volume -", 430);
    createButton(5, "Volume +", 430);
    
    // Adjust Volume Buttons layout specifically
    // Button 4 is "Volume -" (index 4 in vector)
    buttons[4].rect.setSize(sf::Vector2f(100, 40));
    buttons[4].rect.setPosition(WINDOW_WIDTH/2.0f - 120, 430);

    // Button 5 is "Volume +" (index 5 in vector)
    buttons[5].rect.setSize(sf::Vector2f(100, 40));
    buttons[5].rect.setPosition(WINDOW_WIDTH/2.0f + 20, 430);

    // Back Button
    backButtonRect.setSize(sf::Vector2f(200, 50));
    backButtonRect.setFillColor(sf::Color(200, 50, 50));
    backButtonRect.setPosition(WINDOW_WIDTH/2.0f - 100, 600);
    
    backButtonText.setFont(font);
    backButtonText.setString("BACK");
    backButtonText.setCharacterSize(24);
    sf::FloatRect b = backButtonText.getLocalBounds();
    backButtonText.setOrigin(b.width / 2.0f, b.height / 2.0f);
    backButtonText.setPosition(WINDOW_WIDTH/2.0f, 625);
}

void SettingsScreen::createButton(int id, const std::string& label, float y) {
    SettingButton btn;
    btn.id = id;
    btn.labelPrefix = label;
    
    // Default size (overridden for vol buttons later)
    btn.rect.setSize(sf::Vector2f(400, 40));
    btn.rect.setPosition(WINDOW_WIDTH/2.0f - 200, y);
    btn.rect.setFillColor(sf::Color(60, 60, 60));
    
    btn.text.setFont(font);
    btn.text.setCharacterSize(20);
    btn.text.setFillColor(sf::Color::White);
    
    buttons.push_back(btn);
}

void SettingsScreen::updateTexts() {
    for (auto& btn : buttons) {
        std::string suffix = "";
        
        if (btn.id == 0) { // Board Theme
            if (settings.boardThemeIndex == 0) suffix = "Wood";
            else if (settings.boardThemeIndex == 1) suffix = "Blue";
            else suffix = "Dark";
        } 
        else if (btn.id == 1) suffix = textureMgr.getStyleName(settings.stoneStyleIndex);
        else if (btn.id == 2) suffix = settings.soundEnabled ? "ON" : "OFF";
        else if (btn.id == 3) suffix = settings.musicEnabled ? "ON" : "OFF";
        
        // Only append suffix for the first 4 buttons. Volume buttons don't change text.
        if (btn.id < 4) {
            btn.text.setString(btn.labelPrefix + suffix);
        } else {
            btn.text.setString(btn.labelPrefix);
        }

        // Recenter text
        sf::FloatRect b = btn.text.getLocalBounds();
        btn.text.setOrigin(b.width/2.0f, b.height/2.0f);
        btn.text.setPosition(
            btn.rect.getPosition().x + btn.rect.getSize().x/2.0f, 
            btn.rect.getPosition().y + btn.rect.getSize().y/2.0f
        );
    }
}

void SettingsScreen::draw(sf::RenderWindow& window) {
    updateTexts(); // Update labels before drawing (e.g. ON/OFF toggles)
    
    window.draw(background);
    window.draw(titleText);

    for (const auto& btn : buttons) {
        window.draw(btn.rect);
        window.draw(btn.text);
    }
    
    // Draw separate Volume Indicator text
    sf::Text volText("Vol: " + std::to_string((int)settings.volume) + "%", font, 24);
    volText.setPosition(WINDOW_WIDTH/2.0f - 40, 480);
    window.draw(volText);

    window.draw(backButtonRect);
    window.draw(backButtonText);
}

GameState SettingsScreen::handleClick(const sf::Vector2i& mousePos) {
    sf::Vector2f pos(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    
    if (backButtonRect.getGlobalBounds().contains(pos)) {
        return GameState::MENU;
    }

    for (auto& btn : buttons) {
        if (btn.rect.getGlobalBounds().contains(pos)) {
            // Toggle Logic
            if (btn.id == 0) settings.boardThemeIndex = (settings.boardThemeIndex + 1) % 3;
            else if (btn.id == 1) settings.stoneStyleIndex = (settings.stoneStyleIndex + 1) % 3;
            else if (btn.id == 2) settings.soundEnabled = !settings.soundEnabled;
            else if (btn.id == 3) {
                settings.musicEnabled = !settings.musicEnabled;
                audioMgr.updateMusicState();
            }
            else if (btn.id == 4) { // Vol -
                settings.volume = std::max(0.0f, settings.volume - 10.0f);
                audioMgr.updateMusicState();
            }
            else if (btn.id == 5) { // Vol +
                settings.volume = std::min(100.0f, settings.volume + 10.0f);
                audioMgr.updateMusicState();
            }
        }
    }
    return GameState::SETTINGS; // Stay on settings screen
}