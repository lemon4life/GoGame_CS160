#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager(GameSettings& s) : settings(s) {
    // 1. Load Sound Effects
    // These files must exist in cmake-build-debug/assets/audio/
    if (!placeBuffer.loadFromFile("assets/audio/click.mp3")) {
        std::cerr << "[AudioManager] Error: Could not load assets/audio/click.mp3" << std::endl;
    }

    if (!captureBuffer.loadFromFile("assets/audio/capture.mp3")) {
        std::cerr << "[AudioManager] Error: Could not load assets/audio/capture.mp3" << std::endl;
    }

    // 2. Load Background Music
    // OpenFromFile streams it, so it doesn't use much RAM
    if (!bgMusic.openFromFile("assets/audio/bgm.mp3")) {
        std::cerr << "[AudioManager] Error: Could not load assets/audio/bgm.mp3" << std::endl;
    } else {
        if (bgMusic.getStatus() != sf::Music::Playing) {
            bgMusic.play();
        }
        bgMusic.setLoop(true); // Make it repeat forever
    }
}

void AudioManager::playPlaceStone() {
    if (!settings.soundEnabled) return;

    soundEffect.setBuffer(placeBuffer);
    soundEffect.setVolume(settings.volume);
    soundEffect.play();
}

void AudioManager::playCapture() {
    if (!settings.soundEnabled) return;

    soundEffect.setBuffer(captureBuffer);
    soundEffect.setVolume(settings.volume);
    soundEffect.play();
}

void AudioManager::updateMusicState() {
    // Always update volume in case the slider moved
    bgMusic.setVolume(settings.volume);

    // Logic: "Should I be playing?" vs "Am I playing?"
    if (settings.musicEnabled) {
        if (bgMusic.getStatus() != sf::Music::Playing) {
            bgMusic.play();
        }
    } else {
        // If music is disabled but still playing, stop it
        if (bgMusic.getStatus() == sf::Music::Playing) {
            bgMusic.stop();
        }
    }
}