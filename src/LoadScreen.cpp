#include "../include/SaveLoadScreen.h"
#include <cstdio> // For std::remove

void LoadScreen::updateActionButtonState() {
    // If no slot is selected, hide the button
    if (m_selectedSlotId == -1) {
        m_showActionBtn = false;
        return;
    }
    m_showActionBtn = true;

    // Check if the selected slot is empty
    bool isEmpty = true;
    for (const auto& s : m_slots) {
        if (s.id == m_selectedSlotId) isEmpty = s.isEmpty;
    }

    // Update Button Appearance based on state
    if (isEmpty) {
        m_actionBtn->setText("EMPTY SLOT");
        m_actionBtn->setFillColor(Theme::BtnDisabled); // Grey
    } else {
        m_actionBtn->setText("CONTINUE");
        m_actionBtn->setFillColor(Theme::BtnSuccess); // Green
    }
}

int LoadScreen::handleMouseClick(const sf::Vector2i& mousePos) {
    // 1. Check Back Button
    if (m_backButton->isClicked(mousePos)) return -2;

    // 2. Check Action Button (Load Game)
    if (m_showActionBtn && m_actionBtn->isClicked(mousePos)) {
        // Double-check emptiness to prevent loading invalid files
        bool isEmpty = true;
        for (const auto& s : m_slots) {
            if (s.id == m_selectedSlotId) isEmpty = s.isEmpty;
        }

        if (isEmpty) return -1; // Do nothing if empty
        return m_selectedSlotId; // Return ID to main.cpp to trigger load
    }

    // 3. Check Slots (Selection & Deletion)
    for (const auto& slot : m_slots) {
        // Delete Button (Only active if slot is not empty)
        if (!slot.isEmpty && slot.delBtn->isClicked(mousePos)) {
            std::remove(getFilePath(slot.id).c_str());
            refreshSlots();
            if (m_selectedSlotId == slot.id) updatePreview(slot.id);
            return -1;
        }

        // Slot Selection Button
        if (slot.slotBtn->isClicked(mousePos)) {
            updatePreview(slot.id);
            return -1;
        }
    }
    return -1;
}