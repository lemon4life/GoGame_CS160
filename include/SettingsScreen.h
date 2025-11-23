#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Definitions.h"
#include "TextureManager.h"
#include "AudioManager.h"

class SettingsScreen {
private:
    sf::Font font;
    sf::Text titleText;
    sf::RectangleShape background;
    
    struct SettingButton {
        sf::RectangleShape rect;
        sf::Text text;
        std::string labelPrefix;
        int id; // 0:Color, 1:Style, 2:Sound, 3:Music, 4:Vol-, 5:Vol+
    };
    std::vector<SettingButton> buttons;
    
    sf::Text backButtonText;
    sf::RectangleShape backButtonRect;

    // References to systems we need to modify
    GameSettings& settings;
    StoneTextureManager& textureMgr;
    AudioManager& audioMgr;

    void createButton(int id, const std::string& label, float y);
    void updateTexts();

public:
    SettingsScreen(GameSettings& s, StoneTextureManager& tm, AudioManager& am);

    void draw(sf::RenderWindow& window);
    bool handleClick(const sf::Vector2i& mousePos);
};