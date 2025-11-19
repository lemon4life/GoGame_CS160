#include "TextureManager.h"
#include <iostream>

StoneTextureManager::StoneTextureManager() {
    // Load styles here (simplified for example)
    sf::Texture b, w;
    // You should implement the loop to load sets like in the previous code
    if (!b.loadFromFile("assets/img/blackstone.png")) std::cerr << "Error loading black stone\n";
    if (!w.loadFromFile("assets/img/whitestone.png")) std::cerr << "Error loading white stone\n";
    stoneSets.push_back({b, w}); // Classic set
}

const sf::Texture& StoneTextureManager::getTexture(Stone type, int styleIndex) const {
    // Safety check
    if (styleIndex < 0 || styleIndex >= stoneSets.size()) styleIndex = 0;
    if (type == Stone::Black) return stoneSets[styleIndex].first;
    return stoneSets[styleIndex].second;
}

std::string StoneTextureManager::getStyleName(int index) {
    if (index == 0) return "Classic";
    return "Unknown";
}