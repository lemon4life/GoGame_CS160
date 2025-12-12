#ifndef SAVELOADSCREEN_H
#define SAVELOADSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory> // Required for unique_ptr
#include "Board.h"
#include "TextureManager.h"
#include "Theme.h" // Include your new theme

class BaseSlotScreen {
protected:
    sf::RectangleShape m_background;
    sf::Text m_titleText;

    // Replaced raw shapes with Smart Pointers to Buttons
    std::unique_ptr<Theme::Button> m_backButton;
    std::unique_ptr<Theme::Button> m_actionBtn;

    bool m_showActionBtn;

    struct SaveSlot {
        int id;
        bool isEmpty;
        // Each slot has a main button and a delete button
        std::shared_ptr<Theme::Button> slotBtn;
        std::shared_ptr<Theme::Button> delBtn;
    };
    std::vector<SaveSlot> m_slots;

    // Preview Variables
    GoEngine m_previewEngine;
    bool m_showPreview;
    int m_selectedSlotId;
    sf::RectangleShape m_previewBoardBg;
    sf::Text m_infoText;

public:
    virtual ~BaseSlotScreen() = default;
    void init(const sf::Font& font, float width, float height, const std::string& title);
    void refreshSlots();
    void draw(sf::RenderWindow& window, const StoneTextureManager& textureManager);
    virtual int handleMouseClick(const sf::Vector2i& mousePos) = 0;
    void drawPreviewBoard(sf::RenderWindow& window, const StoneTextureManager& tm);
    void updatePreview(int slotId);
    void reload();

protected:
    std::string getFilePath(int id);
    virtual void updateActionButtonState() = 0;
};

class SaveScreen : public BaseSlotScreen {
public:
    int handleMouseClick(const sf::Vector2i& mousePos) override;
protected:
    void updateActionButtonState() override;
};

class LoadScreen : public BaseSlotScreen {
public:
    int handleMouseClick(const sf::Vector2i& mousePos) override;
protected:
    void updateActionButtonState() override;
};

#endif