#ifndef SAVELOADSCREEN_H
#define SAVELOADSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Board.h"
#include "TextureManager.h"

// --- BASE CLASS (Shared UI & Logic) ---
class BaseSlotScreen {
protected:
    // UI Elements
    sf::RectangleShape m_background;
    sf::Text m_titleText;
    sf::RectangleShape m_backButton;
    sf::Text m_backButtonText;

    sf::RectangleShape m_actionBtn;
    sf::Text m_actionBtnText;
    bool m_showActionBtn;

    struct SaveSlot {
        int id;
        bool isEmpty;
        sf::RectangleShape shape;
        sf::Text labelText;
        sf::RectangleShape delBtn;
        sf::Text delBtnText;
    };
    std::vector<SaveSlot> m_slots;

    // Preview
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
    void handleMouseMove(const sf::Vector2i& mousePos);
    virtual int handleMouseClick(const sf::Vector2i& mousePos) = 0;

protected:
    std::string getFilePath(int id);
    void updatePreview(int slotId);
    void drawPreviewBoard(sf::RenderWindow& window, const StoneTextureManager& tm);
    virtual void updateActionButtonState() = 0;
};

// --- SAVE SCREEN SUBCLASS ---
class SaveScreen : public BaseSlotScreen { // <--- Fixed line
public:
    int handleMouseClick(const sf::Vector2i& mousePos) override;
protected:
    void updateActionButtonState() override;
};

// --- LOAD SCREEN SUBCLASS ---
class LoadScreen : public BaseSlotScreen {
public:
    int handleMouseClick(const sf::Vector2i& mousePos) override;
protected:
    void updateActionButtonState() override;
};

#endif