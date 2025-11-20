#ifndef SAVELOADSCREEN_H
#define SAVELOADSCREEN_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SaveLoadScreen {
private:
    sf::RectangleShape m_background; // Dark overlay
    sf::Text m_titleText;
    sf::RectangleShape m_backButton;
    sf::Text m_backButtonText;

    struct SaveSlot {
        int id;
        bool isEmpty;
        sf::RectangleShape shape;   // The bar
        sf::Text labelText;         // "Save Slot 1"
        sf::RectangleShape delBtn;  // Red square
        sf::Text delBtnText;        // "X"
    };

    std::vector<SaveSlot> m_slots;
    bool m_isSaveMode; // true = Save, false = Load

public:
    void init(const sf::Font& font, float width, float height);
    void setMode(bool isSave); // Switch between "SAVE GAME" and "LOAD GAME" titles
    void refreshSlots();       // Check file system
    
    void draw(sf::RenderWindow& window);
    
    // Returns: 
    //  -1 if nothing clicked
    //  -2 if Back button clicked
    //   1-5 if a slot was clicked (to save or load)
    int handleMouseClick(const sf::Vector2i& mousePos);

private:
    std::string getFilePath(int id);
};

#endif