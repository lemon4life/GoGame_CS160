#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Definitions.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "Theme.h"

class SettingsScreen {
private:
    GameSettings& settings;
    StoneTextureManager& textureMgr;
    AudioManager& audioMgr;

    sf::Font font;
    sf::Text titleText;
    sf::RectangleShape background;

    // Volume Indicator Text
    sf::Text volumeText;

    // The Back Button
    std::unique_ptr<Theme::Button> backButton;

    // Generic Setting Button Structure
    struct SettingBtn {
        std::unique_ptr<Theme::Button> btn;
        int id; // 0=Theme, 1=Style, 2=Sound, 3=Music, 4=Vol-, 5=Vol+, 6=BGM
        std::string labelPrefix;
    };
    std::vector<SettingBtn> m_buttons;

    // Helper to create buttons easily
    void addButton(int id, const std::string& label, float y, float width = 400.0f, float xOffset = 0.0f);

public:
    SettingsScreen(GameSettings& s, StoneTextureManager& tm, AudioManager& am);

    void updateTexts(); // Synchronizes button text with current settings
    void draw(sf::RenderWindow& window);
    bool handleClick(const sf::Vector2i& mousePos);
};

#endif