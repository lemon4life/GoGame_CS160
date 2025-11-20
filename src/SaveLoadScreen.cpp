#include "SaveLoadScreen.h"
#include <filesystem> // C++17
#include <iostream>
#include <cstdio> // For std::remove

namespace fs = std::filesystem;

void SaveLoadScreen::init(const sf::Font& font, float width, float height) {
    // 1. Background Overlay (Semi-transparent)
    m_background.setSize(sf::Vector2f(width, height));
    m_background.setFillColor(sf::Color(0, 0, 0, 220));

    // 2. Title
    m_titleText.setFont(font);
    m_titleText.setCharacterSize(40);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition(50, 40);

    // 3. Back Button
    m_backButton.setSize(sf::Vector2f(100, 40));
    m_backButton.setFillColor(sf::Color(100, 100, 100));
    m_backButton.setPosition(width - 150, 40);
    m_backButton.setOutlineThickness(2);
    m_backButton.setOutlineColor(sf::Color::White);

    m_backButtonText.setFont(font);
    m_backButtonText.setString("Back");
    m_backButtonText.setCharacterSize(20);
    m_backButtonText.setFillColor(sf::Color::White);
    // Center text roughly
    m_backButtonText.setPosition(width - 125, 48);

    // 4. Initialize 5 Slots
    float startY = 120;
    for (int i = 1; i <= 5; ++i) {
        SaveSlot slot;
        slot.id = i;
        
        // Main Bar
        slot.shape.setSize(sf::Vector2f(400, 60));
        slot.shape.setPosition(100, startY);
        slot.shape.setOutlineThickness(2);
        slot.shape.setOutlineColor(sf::Color(200, 200, 200));

        // Label
        slot.labelText.setFont(font);
        slot.labelText.setCharacterSize(24);
        slot.labelText.setFillColor(sf::Color::White);
        slot.labelText.setPosition(120, startY + 15);

        // Delete Button (Red X)
        slot.delBtn.setSize(sf::Vector2f(60, 60)); // Square
        slot.delBtn.setPosition(520, startY); // Right next to bar
        slot.delBtn.setFillColor(sf::Color(200, 50, 50)); // Red
        slot.delBtn.setOutlineThickness(2);
        slot.delBtn.setOutlineColor(sf::Color::White);

        slot.delBtnText.setFont(font);
        slot.delBtnText.setString("X");
        slot.delBtnText.setCharacterSize(30);
        slot.delBtnText.setFillColor(sf::Color::White);
        slot.delBtnText.setPosition(540, startY + 10);

        m_slots.push_back(slot);
        startY += 80; // Spacing
    }
}

void SaveLoadScreen::setMode(bool isSave) {
    m_isSaveMode = isSave;
    m_titleText.setString(isSave ? "SAVE GAME" : "LOAD GAME");
    refreshSlots();
}

std::string SaveLoadScreen::getFilePath(int id) {
    return "saves/save_0" + std::to_string(id) + ".txt";
}

void SaveLoadScreen::refreshSlots() {
    // Create directory if missing
    if (!fs::exists("saves")) {
        fs::create_directory("saves");
    }

    for (auto& slot : m_slots) {
        std::string path = getFilePath(slot.id);
        if (fs::exists(path)) {
            slot.isEmpty = false;
            slot.shape.setFillColor(sf::Color(100, 150, 100)); // Green for occupied
            slot.labelText.setString("Save Slot " + std::to_string(slot.id) + " (Occupied)");
        } else {
            slot.isEmpty = true;
            slot.shape.setFillColor(sf::Color(60, 60, 60)); // Grey for empty
            slot.labelText.setString("Save Slot " + std::to_string(slot.id) + " (Empty)");
        }
    }
}

void SaveLoadScreen::draw(sf::RenderWindow& window) {
    window.draw(m_background);
    window.draw(m_titleText);
    window.draw(m_backButton);
    window.draw(m_backButtonText);

    for (const auto& slot : m_slots) {
        window.draw(slot.shape);
        window.draw(slot.labelText);
        
        // Only show delete button if file exists
        if (!slot.isEmpty) {
            window.draw(slot.delBtn);
            window.draw(slot.delBtnText);
        }
    }
}

int SaveLoadScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    // 1. Check Back Button
    if (m_backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        return -2; // Back signal
    }

    // 2. Check Slots
    for (const auto& slot : m_slots) {
        float mx = static_cast<float>(mousePos.x);
        float my = static_cast<float>(mousePos.y);

        // Check Delete Button (Only if not empty)
        if (!slot.isEmpty && slot.delBtn.getGlobalBounds().contains(mx, my)) {
            std::string path = "saves/save_0" + std::to_string(slot.id) + ".txt";
            std::remove(path.c_str());
            refreshSlots(); // Update UI immediately
            return -1; // Handled internally, no main loop action needed
        }

        // Check Slot Bar Click
        if (slot.shape.getGlobalBounds().contains(mx, my)) {
            // If loading, only allow clicking occupied slots
            if (!m_isSaveMode && slot.isEmpty) return -1;
            
            return slot.id; // Return the ID so main loop can save/load
        }
    }
    return -1;
}