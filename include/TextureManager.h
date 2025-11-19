#pragma once
#include "Definitions.h"
#include <vector>

class StoneTextureManager {
private:
    std::vector<std::pair<sf::Texture, sf::Texture>> stoneSets;
public:
    StoneTextureManager();
    const sf::Texture& getTexture(Stone type, int styleIndex) const;
    std::string getStyleName(int index);
};