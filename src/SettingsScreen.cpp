#include "../include/SettingsScreen.h"
#include <iostream>
#include <algorithm>

SettingsScreen::SettingsScreen(GameSettings& s, StoneTextureManager& tm, AudioManager& am)
    : settings(s), textureMgr(tm), audioMgr(am)
{
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        std::cerr << "[Settings] Error loading font" << std::endl;
    }

    // 1. Setup Background & Title
    background.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    background.setFillColor(Theme::Background);

    titleText.setFont(font);
    titleText.setString("SETTINGS");
    titleText.setCharacterSize(40);
    titleText.setFillColor(Theme::TextNormal);

    // Center Title
    sf::FloatRect tb = titleText.getLocalBounds();
    titleText.setOrigin(tb.width / 2.0f, tb.height / 2.0f);
    titleText.setPosition(WINDOW_WIDTH / 2.0f, 100);

    // 2. Setup Volume Text Display
    volumeText.setFont(font);
    volumeText.setCharacterSize(24);
    volumeText.setFillColor(Theme::TextNormal);
    volumeText.setPosition(WINDOW_WIDTH / 2.0f - 40, 535);

    // 3. Setup Buttons using Helper
    // ID mapping: 0:Theme, 1:Style, 2:Sound, 3:Music, 6:BGM, 4:Vol-, 5:Vol+

    float startY = 200.0f;
    float gap = 70.0f;

    addButton(0, "Board Theme: ", startY);
    addButton(1, "Stone Style: ", startY + gap);
    addButton(2, "Sound Effects: ", startY + gap * 2);
    addButton(3, "Music: ", startY + gap * 3);
    addButton(6, "BGM Track: ", startY + gap * 4);

    // Volume Buttons (Smaller, Side-by-Side)
    float volY = 550.0f;
    addButton(4, "Vol -", volY, 100.0f, -120.0f); // Left
    addButton(5, "Vol +", volY, 100.0f, 120.0f);  // Right

    // 4. Setup Back Button
    backButton = std::make_unique<Theme::Button>(
        font, "BACK",
        sf::Vector2f(200, 50),
        sf::Vector2f(WINDOW_WIDTH / 2.0f, 620),
        Theme::BtnDanger
    );
}

void SettingsScreen::addButton(int id, const std::string& label, float y, float width, float xOffset) {
    auto btn = std::make_unique<Theme::Button>(
        font, label,
        sf::Vector2f(width, 40),
        sf::Vector2f(WINDOW_WIDTH / 2.0f + xOffset, y),
        Theme::BtnDisabled // Use Grey for settings toggles
    );

    m_buttons.push_back({std::move(btn), id, label});
}

void SettingsScreen::updateTexts() {
    // 1. Update Dynamic Labels
    for (auto& item : m_buttons) {
        std::string suffix = "";

        if (item.id == 0) { // Theme
            if (settings.boardThemeIndex == 0) suffix = "Wood";
            else if (settings.boardThemeIndex == 1) suffix = "Ocean";
            else suffix = "Galaxy";
            item.btn->setText(item.labelPrefix + suffix);
        }
        else if (item.id == 1) { // Style
            suffix = textureMgr.getStyleName(settings.stoneStyleIndex);
            item.btn->setText(item.labelPrefix + suffix);
        }
        else if (item.id == 2) { // Sound
            suffix = settings.soundEnabled ? "ON" : "OFF";
            item.btn->setText(item.labelPrefix + suffix);
            // Optional: Visual feedback (Green for ON, Red for OFF)
            item.btn->setOutlineColor(settings.soundEnabled ? Theme::BtnSuccess : Theme::BtnDanger);
        }
        else if (item.id == 3) { // Music
            suffix = settings.musicEnabled ? "ON" : "OFF";
            item.btn->setText(item.labelPrefix + suffix);
            item.btn->setOutlineColor(settings.musicEnabled ? Theme::BtnSuccess : Theme::BtnDanger);
        }
        else if (item.id == 6) { // BGM Track
            suffix = audioMgr.getCurrentTrackName();
            item.btn->setText(item.labelPrefix + suffix);
        }
        // Vol- and Vol+ (IDs 4, 5) text doesn't change
    }

    // 2. Update Volume Text
    volumeText.setString("Vol: " + std::to_string((int)settings.volume) + "%");
}

void SettingsScreen::draw(sf::RenderWindow& window) {
    updateTexts(); // Refresh logic before drawing

    window.draw(background);
    window.draw(titleText);

    for (const auto& item : m_buttons) {
        item.btn->draw(window);
    }

    window.draw(volumeText);
    backButton->draw(window);
}

bool SettingsScreen::handleClick(const sf::Vector2i& mousePos) {
    // 1. Check Back Button
    if (backButton->isClicked(mousePos)) {
        return true; // Signal to exit settings
    }

    // 2. Check Setting Toggles
    for (auto& item : m_buttons) {
        if (item.btn->isClicked(mousePos)) {
            // Apply Logic based on ID
            switch (item.id) {
                case 0: // Theme
                    settings.boardThemeIndex = (settings.boardThemeIndex + 1) % 3;
                    break;
                case 1: // Stone Style
                    settings.stoneStyleIndex = (settings.stoneStyleIndex + 1) % STONESNUM;
                    break;
                case 2: // Sound
                    settings.soundEnabled = !settings.soundEnabled;
                    break;
                case 3: // Music
                    settings.musicEnabled = !settings.musicEnabled;
                    audioMgr.updateMusicState();
                    break;
                case 4: // Vol -
                    settings.volume = std::max(0.0f, settings.volume - 10.0f);
                    audioMgr.updateMusicState();
                    break;
                case 5: // Vol +
                    settings.volume = std::min(100.0f, settings.volume + 10.0f);
                    audioMgr.updateMusicState();
                    break;
                case 6: // BGM Track
                    int count = audioMgr.getTrackCount();
                    if (count > 0) {
                        settings.bgmIndex = (settings.bgmIndex + 1) % count;
                        audioMgr.changeBGM(settings.bgmIndex);
                    }
                    break;
            }
            // Break loop after clicking one button to prevent overlap issues
            return false;
        }
    }
    return false;
}