#include "SaveLoadScreen.h"
#include <filesystem>
#include <cstdio>
#include <iostream>

namespace fs = std::filesystem;

const float PREVIEW_X = 600.0f;
const float PREVIEW_Y = 150.0f;
const float PREVIEW_SIZE = 350.0f;
const float SLOT_START_Y = 150.0f;

// --- BASE CLASS IMPLEMENTATION ---

void BaseSlotScreen::init(const sf::Font& font, float width, float height, const std::string& title) {
    // 1. Setup UI Elements
    m_background.setSize(sf::Vector2f(width, height));
    m_background.setFillColor(sf::Color(30, 30, 30, 240));

    m_titleText.setFont(font);
    m_titleText.setCharacterSize(50);
    m_titleText.setFillColor(sf::Color(220, 220, 220));
    m_titleText.setPosition(80, 50);
    m_titleText.setString(title);

    m_backButton.setSize(sf::Vector2f(100, 40));
    m_backButton.setFillColor(sf::Color(100, 150, 100));
    m_backButton.setPosition(width - 150, 60);

    m_backButtonText.setFont(font);
    m_backButtonText.setString("Back");
    m_backButtonText.setCharacterSize(20);
    m_backButtonText.setFillColor(sf::Color::White);
    m_backButtonText.setPosition(width - 130, 68);

    m_previewBoardBg.setSize(sf::Vector2f(PREVIEW_SIZE + 20, PREVIEW_SIZE + 20));
    m_previewBoardBg.setPosition(PREVIEW_X - 10, PREVIEW_Y - 10);
    m_previewBoardBg.setFillColor(sf::Color(222, 184, 135));

    m_infoText.setFont(font);
    m_infoText.setCharacterSize(20);
    m_infoText.setFillColor(sf::Color::White);
    m_infoText.setPosition(PREVIEW_X, PREVIEW_Y + PREVIEW_SIZE + 20);

    m_actionBtn.setSize(sf::Vector2f(300, 60));
    m_actionBtn.setPosition(PREVIEW_X + 25, PREVIEW_Y + PREVIEW_SIZE + 80);

    m_actionBtnText.setFont(font);
    m_actionBtnText.setCharacterSize(20);
    m_actionBtnText.setFillColor(sf::Color::White);

    // 2. Initialize Slots
    float currentY = SLOT_START_Y;
    m_slots.clear();
    for (int i = 1; i <= 5; ++i) {
        SaveSlot slot;
        slot.id = i;
        slot.shape.setSize(sf::Vector2f(300, 50));
        slot.shape.setPosition(80, currentY);
        slot.shape.setOutlineThickness(2);
        slot.labelText.setFont(font);
        slot.labelText.setCharacterSize(20);
        slot.labelText.setPosition(100, currentY + 12);

        slot.delBtn.setSize(sf::Vector2f(40, 40));
        slot.delBtn.setPosition(400, currentY + 5);
        slot.delBtn.setFillColor(sf::Color(200, 60, 60));

        slot.delBtnText.setFont(font);
        slot.delBtnText.setString("X");
        slot.delBtnText.setCharacterSize(20);
        slot.delBtnText.setPosition(412, currentY + 8);

        m_slots.push_back(slot);
        currentY += 70;
    }

    // 3. CRITICAL: Initialize Preview Engine
    // If this is missing, loadGame() on preview will crash
    m_previewEngine.initialize_board(19);

    m_showPreview = false;
    m_showActionBtn = false;
    m_selectedSlotId = -1;
}

void BaseSlotScreen::reload() {
    m_showPreview = false;
    m_showActionBtn = false;
    m_selectedSlotId = -1;
}

std::string BaseSlotScreen::getFilePath(int id) {
    return "saves/save_0" + std::to_string(id) + ".txt";
}

void BaseSlotScreen::refreshSlots() {
    if (!fs::exists("saves")) fs::create_directory("saves");

    for (auto& slot : m_slots) {
        std::string path = getFilePath(slot.id);
        if (fs::exists(path)) {
            slot.isEmpty = false;
            slot.shape.setFillColor(sf::Color(60, 60, 60));
            slot.labelText.setString("save_0" + std::to_string(slot.id));
            slot.labelText.setFillColor(sf::Color::White);
        } else {
            slot.isEmpty = true;
            slot.shape.setFillColor(sf::Color(40, 40, 40));
            slot.labelText.setString("Empty Slot");
            slot.labelText.setFillColor(sf::Color(100, 100, 100));
        }

        if (slot.id == m_selectedSlotId) slot.shape.setOutlineColor(sf::Color::Yellow);
        else slot.shape.setOutlineColor(sf::Color(100, 100, 100));
    }
}

void BaseSlotScreen::updatePreview(int slotId) {
    // If we click a different slot, reload preview
    if (slotId != m_selectedSlotId) {
        m_selectedSlotId = slotId;
        std::string path = getFilePath(slotId);

        if (fs::exists(path)) {
            // Try loading. If preview engine wasn't init, this might crash.
            if (m_previewEngine.loadGame(path)) {
                m_showPreview = true;
                std::string pName = (m_previewEngine.getCurrentPlayer() == Player::BLACK) ? "Black" : "White";
                m_infoText.setString("Turn: " + pName + "\n2-Player Mode");
            }
        } else {
            m_showPreview = false;
            m_infoText.setString("Empty Slot");
        }
    }
    // Always refresh UI colors and button text
    refreshSlots();
    updateActionButtonState();
}

void BaseSlotScreen::drawPreviewBoard(sf::RenderWindow& window, const StoneTextureManager& tm) {
    window.draw(m_previewBoardBg);
    if (!m_showPreview) return;

    int boardSize = 19;
    float cellSize = PREVIEW_SIZE / (boardSize + 1);
    float stoneRadius = cellSize * 0.45f;

    sf::RectangleShape line;
    line.setFillColor(sf::Color::Black);

    for (int i = 1; i <= boardSize; ++i) {
        float pos = i * cellSize;
        line.setSize(sf::Vector2f(2, PREVIEW_SIZE - 2 * cellSize));
        line.setPosition(PREVIEW_X + pos, PREVIEW_Y + cellSize);
        window.draw(line);
        line.setSize(sf::Vector2f(PREVIEW_SIZE - 2 * cellSize, 2));
        line.setPosition(PREVIEW_X + cellSize, PREVIEW_Y + pos);
        window.draw(line);
    }

    const auto& board = m_previewEngine.getBoard();
    sf::CircleShape stone(stoneRadius);
    stone.setOrigin(stoneRadius, stoneRadius);

    // Pointer fix for textures
    const sf::Texture* blackTex = &tm.getTexture(Stone::Black, 0);
    const sf::Texture* whiteTex = &tm.getTexture(Stone::White, 0);

    for (int y = 1; y <= boardSize; ++y) {
        for (int x = 1; x <= boardSize; ++x) {
            // Safety check for vector bounds
            if (y >= board.size() || x >= board[y].size()) continue;

            Player p = board[x][y];
            if (p == Player::NONE) continue;

            if (p == Player::BLACK) stone.setTexture(blackTex);
            else stone.setTexture(whiteTex);

            stone.setPosition(PREVIEW_X + x * cellSize, PREVIEW_Y + y * cellSize);
            window.draw(stone);
        }
    }
}

void BaseSlotScreen::draw(sf::RenderWindow& window, const StoneTextureManager& tm) {
    window.draw(m_background);
    window.draw(m_titleText);
    window.draw(m_backButton);
    window.draw(m_backButtonText);

    if (m_selectedSlotId != -1) {
        window.draw(m_infoText);
        if (m_showActionBtn) {
            window.draw(m_actionBtn);
            window.draw(m_actionBtnText);
        }
        // Only draw preview if we have one
        if(m_showPreview) {
            drawPreviewBoard(window, tm);
        }
    }

    for (const auto& slot : m_slots) {
        window.draw(slot.shape);
        window.draw(slot.labelText);
        // Only draw delete button if slot occupied
        if (!slot.isEmpty) {
            window.draw(slot.delBtn);
            window.draw(slot.delBtnText);
        }
    }
}

void BaseSlotScreen::handleMouseMove(const sf::Vector2i& mousePos) {
    // Optional hover logic
}