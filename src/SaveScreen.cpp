#include "SaveLoadScreen.h"
#include <cstdio>

// --- SAVE SCREEN IMPLEMENTATION ---

void SaveScreen::updateActionButtonState() {
    if (m_selectedSlotId == -1) {
        m_showActionBtn = false;
        return;
    }
    m_showActionBtn = true;
    
    bool isEmpty = true;
    for (const auto& s : m_slots) if (s.id == m_selectedSlotId) isEmpty = s.isEmpty;

    if (isEmpty) {
        m_actionBtnText.setString("Save Here");
        m_actionBtn.setFillColor(sf::Color(50, 150, 50)); // Green
    } else {
        m_actionBtnText.setString("Overwrite Here");
        m_actionBtn.setFillColor(sf::Color(200, 100, 50)); // Orange
    }

    // Center Text
    sf::FloatRect textRect = m_actionBtnText.getLocalBounds();
    m_actionBtnText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
    m_actionBtnText.setPosition(
        m_actionBtn.getPosition().x + m_actionBtn.getSize().x/2.0f,
        m_actionBtn.getPosition().y + m_actionBtn.getSize().y/2.0f
    );
}

int SaveScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    if (m_backButton.getGlobalBounds().contains(mx, my)) return -2;

    // Action Button (Save/Overwrite)
    if (m_showActionBtn && m_actionBtn.getGlobalBounds().contains(mx, my)) {
        for (const auto& slot : m_slots) {
                std::remove(getFilePath(m_selectedSlotId).c_str());
                refreshSlots();
                if (m_selectedSlotId == slot.id) updatePreview(slot.id);
                return m_selectedSlotId;
            }
            return -1;
        }

    for (const auto& slot : m_slots) {
        // Delete
        if (!slot.isEmpty && slot.delBtn.getGlobalBounds().contains(mx, my)) {
            std::remove(getFilePath(slot.id).c_str());
            refreshSlots();
            if (m_selectedSlotId == slot.id) updatePreview(slot.id);
            return -1;
        }
        // Select
        if (slot.shape.getGlobalBounds().contains(mx, my)) {
            updatePreview(slot.id);
            return -1;
        }
    }
    return -1;
}