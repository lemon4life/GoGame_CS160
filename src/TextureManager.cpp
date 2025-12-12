#include "TextureManager.h"
#include <iostream>

StoneTextureManager::StoneTextureManager() {
    sf::Texture b, w;

    if (!b.loadFromFile("assets/img/stones/glass_blackstone.png")) std::cerr << "Error loading black stone\n";
    if (!w.loadFromFile("assets/img/stones/glass_whitestone.png")) std::cerr << "Error loading white stone\n";
    stoneSets.push_back({b, w}); // Classic set

    if (!b.loadFromFile("assets/img/stones/flat_blackstone.png")) std::cerr << "Error loading black stone\n";
    if (!w.loadFromFile("assets/img/stones/flat_whitestone.png")) std::cerr << "Error loading white stone\n";
    stoneSets.push_back({b, w}); // Classic setif (!b.loadFromFile("assets/img/stones/glass_blackstone.png")) std::cerr << "Error loading black stone\n";

    if (!b.loadFromFile("assets/img/stones/star_blackstone.png")) std::cerr << "Error loading black stone\n";
    if (!w.loadFromFile("assets/img/stones/star_whitestone.png")) std::cerr << "Error loading white stone\n";
    stoneSets.push_back({b, w}); // Classic setif (!b.loadFromFile("assets/img/stones/glass_blackstone.png")) std::cerr << "Error loading black stone\n";
}

const sf::Texture& StoneTextureManager::getTexture(Stone type, int styleIndex) const {
    if (styleIndex < 0 || styleIndex >= stoneSets.size()) styleIndex = 0;
    if (type == Stone::Black) return stoneSets[styleIndex].first;
    return stoneSets[styleIndex].second;
}

std::string StoneTextureManager::getStyleName(int index){
    if (index < 0 || index >= stoneSets.size()) index = 0;
    if (index == 0) return "Glass";
    if (index == 1) return "Flat";
    if (index == 2) return "Star";
}