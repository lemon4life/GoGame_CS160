#include "../include/SaveLoadScreen.h"
#include <cstdio>

void SaveScreen::updateActionButtonState() {
    if (m_selectedSlotId == -1) {
        m_showActionBtn = false;
        return;
    }
    m_showActionBtn = true;

    bool isEmpty = true;
    for (const auto& s : m_slots) if (s.id == m_selectedSlotId) isEmpty = s.isEmpty;

    if (isEmpty) {
        m_actionBtn->setText("SAVE GAME");
        m_actionBtn->setFillColor(Theme::BtnSuccess);
    } else {
        m_actionBtn->setText("OVERWRITE");
        m_actionBtn->setFillColor(sf::Color(200, 100, 0)); // Orange for warning
    }
}

int SaveScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    if (m_backButton->isClicked(mousePos)) return -2;

    if (m_showActionBtn && m_actionBtn->isClicked(mousePos)) {
        // Logic handled in main, just return ID
        return m_selectedSlotId;
    }

    for (const auto& slot : m_slots) {
        // Delete Button
        if (!slot.isEmpty && slot.delBtn->isClicked(mousePos)) {
            std::remove(getFilePath(slot.id).c_str());
            refreshSlots();
            if (m_selectedSlotId == slot.id) updatePreview(slot.id);
            return -1;
        }
        // Slot Select
        if (slot.slotBtn->isClicked(mousePos)) {
            updatePreview(slot.id);
            return -1;
        }
    }
    return -1;
}