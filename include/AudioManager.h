#pragma once // Prevents this file from being included twice
#include <SFML/Audio.hpp>
#include "Definitions.h" // Needed for GameSettings struct

class AudioManager {
private:
    // Buffers hold the actual audio data in memory
    sf::SoundBuffer placeBuffer;
    sf::SoundBuffer captureBuffer;

    // The 'Sound' object is the player that plays the buffer
    sf::Sound soundEffect;

    // Music is streamed from the file (better for long files)
    sf::Music bgMusic;

    // Reference to the global settings so we know the volume/mute state
    GameSettings& settings;

public:
    // Constructor takes a reference to settings
    explicit AudioManager(GameSettings& s);

    // Actions
    void playPlaceStone();
    void playCapture();

    // Checks settings to see if music should start, stop, or change volume
    void updateMusicState();
};