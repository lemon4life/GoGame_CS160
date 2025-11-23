#include "SaveLoadScreen.h"
#include <cstdio>

void LoadScreen::updateActionButtonState() {
    if (m_selectedSlotId == -1) {
        m_showActionBtn = false;
        return;
    }
    m_showActionBtn = true;

    bool isEmpty = true;
    for (const auto& s : m_slots) if (s.id == m_selectedSlotId) isEmpty = s.isEmpty;

    if (isEmpty) {
        m_actionBtnText.setString("No game was saved here");
        m_actionBtn.setFillColor(sf::Color(80, 80, 80)); // Grey Disabled
    } else {
        m_actionBtnText.setString("Continue Playing");
        m_actionBtn.setFillColor(sf::Color(50, 150, 50)); // Green
    }

    sf::FloatRect textRect = m_actionBtnText.getLocalBounds();
    m_actionBtnText.setOrigin(textRect.left + textRect.width/2.0f, textRect.top + textRect.height/2.0f);
    m_actionBtnText.setPosition(
        m_actionBtn.getPosition().x + m_actionBtn.getSize().x/2.0f,
        m_actionBtn.getPosition().y + m_actionBtn.getSize().y/2.0f
    );
}

int LoadScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    float mx = static_cast<float>(mousePos.x);
    float my = static_cast<float>(mousePos.y);

    if (m_backButton.getGlobalBounds().contains(mx, my)) return -2;

    // Action Button (Load)
    if (m_showActionBtn && m_actionBtn.getGlobalBounds().contains(mx, my)) {
        // Check empty again to enforce disabled state
        bool isEmpty = true;
        for (const auto& s : m_slots) if (s.id == m_selectedSlotId) isEmpty = s.isEmpty;

        if (isEmpty) return -1; // Disabled

        return m_selectedSlotId; 
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