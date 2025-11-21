#pragma once
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include "Definitions.h"

class AudioManager {
private:
    GameSettings& settings;

    // Sound Effects
    sf::SoundBuffer placeBuffer;
    sf::SoundBuffer captureBuffer;
    sf::Sound soundEffect;

    // Music
    sf::Music bgMusic;
    std::vector<std::string> bgmFiles; // List of file paths
    std::vector<std::string> bgmNames; // Display names for UI

public:
    AudioManager(GameSettings& s);

    void playPlaceStone();
    void playCapture();

    void updateMusicState(); // Handles Volume & On/Off logic
    void changeBGM(int index); // Handles track switching

    // Helper for UI
    std::string getCurrentTrackName() const;
    int getTrackCount() const;
};