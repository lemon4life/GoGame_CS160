#include "../include/SaveLoadScreen.h"
#include <filesystem>
#include <cstdio>
#include <iostream>

namespace fs = std::filesystem;

// Layout Constants
const float PREVIEW_X = 600.0f ;
const float PREVIEW_Y = 150.0f;
const float PREVIEW_SIZE = 350.0f;
const float SLOT_START_Y = 150.0f;

// --- BASE CLASS IMPLEMENTATION ---

void BaseSlotScreen::init(const sf::Font& font, float width, float height, const std::string& title) {
    // 1. Background
    m_background.setSize(sf::Vector2f(width, height));
    m_background.setFillColor(sf::Color(20, 20, 20, 240)); // Slightly darker

    // 2. Title
    m_titleText.setFont(font);
    m_titleText.setCharacterSize(40);
    m_titleText.setFillColor(Theme::TextNormal);
    m_titleText.setPosition(80, 50);
    m_titleText.setString(title);

    // 3. Back Button (Using Button Class)
    m_backButton = std::make_unique<Theme::Button>(
        font, "BACK", sf::Vector2f(120, 40),
        sf::Vector2f(width - 100, 70), Theme::BtnDanger
    );

    // 4. Action Button (Save/Load) - Initially hidden
    m_actionBtn = std::make_unique<Theme::Button>(
        font, "ACTION", sf::Vector2f(300, 60),
        sf::Vector2f(PREVIEW_X + 175, PREVIEW_Y + PREVIEW_SIZE + 100),
        Theme::BtnSuccess
    );

    // 5. Preview Elements
    m_previewBoardBg.setSize(sf::Vector2f(PREVIEW_SIZE + 20, PREVIEW_SIZE + 20));
    m_previewBoardBg.setPosition(PREVIEW_X - 10, PREVIEW_Y - 10);
    m_previewBoardBg.setFillColor(sf::Color(222, 184, 135)); // Wooden color

    m_infoText.setFont(font);
    m_infoText.setCharacterSize(18);
    m_infoText.setFillColor(Theme::TextNormal);
    m_infoText.setPosition(PREVIEW_X, PREVIEW_Y + PREVIEW_SIZE + 20);

    // 6. Init Slots
    m_slots.clear();
    float currentY = SLOT_START_Y;

    for (int i = 1; i <= 5; ++i) {
        SaveSlot slot;
        slot.id = i;

        // Slot Button (The big rectangle)
        slot.slotBtn = std::make_shared<Theme::Button>(
            font, "Empty Slot", sf::Vector2f(350, 50),
            sf::Vector2f(255, currentY + 25), // Center X is ~255
            Theme::BtnDisabled
        );

        // Delete Button (The small X)
        slot.delBtn = std::make_shared<Theme::Button>(
            font, "X", sf::Vector2f(40, 40),
            sf::Vector2f(480, currentY + 25),
            Theme::BtnDanger
        );

        m_slots.push_back(slot);
        currentY += 70; // Spacing
    }

    m_previewEngine.initialize_board(19);
    reload();
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
            // Update Text and Color for occupied slot
            slot.slotBtn->setText("Save File " + std::to_string(slot.id));
            slot.slotBtn->setFillColor(Theme::BtnPrimary);
        } else {
            slot.isEmpty = true;
            slot.slotBtn->setText("Empty Slot");
            slot.slotBtn->setFillColor(Theme::BtnDisabled);
        }

        // Highlight Selected Slot
        if (slot.id == m_selectedSlotId) {
            slot.slotBtn->setOutlineColor(sf::Color::Yellow);
        } else {
            slot.slotBtn->setOutlineColor(sf::Color::White);
        }
    }
}

void BaseSlotScreen::updatePreview(int slotId) {
    if (slotId != m_selectedSlotId) {
        m_selectedSlotId = slotId;
        std::string path = getFilePath(slotId);

        if (fs::exists(path)) {
            Difficulty savedMode = Difficulty::NONE;
            if (m_previewEngine.loadGame(path, savedMode)) {
                m_showPreview = true;
                std::string pName = (m_previewEngine.getCurrentPlayer() == Player::BLACK) ? "Black" : "White";

                std::string modeStr = "2-Player";

                if (savedMode == Difficulty::EASY) modeStr = "AI (Easy)";
                else if (savedMode == Difficulty::MEDIUM) modeStr = "AI (Medium)";
                else if (savedMode == Difficulty::HARD) modeStr = "AI (Hard)";

                m_infoText.setString("Mode: " + modeStr + " | Turn: " + pName);
            }
        } else {
            m_showPreview = false;
            m_infoText.setString("Empty Slot");
        }
    }
    refreshSlots();
    updateActionButtonState();
}

void BaseSlotScreen::drawPreviewBoard(sf::RenderWindow& window, const StoneTextureManager& tm) {
    if (!m_showPreview) return;
    window.draw(m_previewBoardBg);

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
    m_backButton->draw(window);

    // Draw Slots
    for (const auto& slot : m_slots) {
        slot.slotBtn->draw(window);
        if (!slot.isEmpty) {
            slot.delBtn->draw(window);
        }
    }

    // Draw Preview
    if (m_selectedSlotId != -1) {
        drawPreviewBoard(window, tm);
        window.draw(m_infoText);
        if (m_showActionBtn) {
            m_actionBtn->draw(window);
        }
    }
}