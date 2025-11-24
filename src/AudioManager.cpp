#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager(GameSettings& s) : settings(s) {
    //Load Sound Effects
    if (!placeBuffer.loadFromFile("assets/audio/effects/click.mp3")) {
        std::cerr << "[AudioManager] Error: click.mp3 missing" << std::endl;
    }
    if (!captureBuffer.loadFromFile("assets/audio/effects/capture.mp3")) {
        std::cerr << "[AudioManager] Error: capture.mp3 missing" << std::endl;
    }

    if (!errorBuffer.loadFromFile("assets/audio/effects/rizz.mp3")) {
        std::cerr << "[AudioManager] Error:rizz.mp3 missing" << std::endl;
    }

    //Setup BGM Lists
    bgmFiles = {
        "assets/audio/BGM/Judgment_of_Euthymia.mp3",
        "assets/audio/BGM/The_Listener.mp3",
    };

    bgmNames = {
        "Judgment of Euthymia",
        "The Listener",
    };

    //Start Initial BGM
    changeBGM(settings.bgmIndex);
}

void AudioManager::playPlaceStone() {
    if (!settings.soundEnabled) return;
    soundEffect.setBuffer(placeBuffer);
    //soundEffect.setVolume(settings.volume);
    soundEffect.play();
}

void AudioManager::playError() {
    if (!settings.soundEnabled) return;
    soundEffect.setBuffer(errorBuffer);
    //soundEffect.setVolume(settings.volume);
    soundEffect.play();
}

void AudioManager::playCapture() {
    if (!settings.soundEnabled) return;
    soundEffect.setBuffer(captureBuffer);
    //soundEffect.setVolume(settings.volume);
    soundEffect.play();
}

void AudioManager::changeBGM(int index) {
    if (index < 0 || index >= bgmFiles.size()) return;

    // Stop current music
    bgMusic.stop();

    // Load new file
    if (!bgMusic.openFromFile(bgmFiles[index])) {
        std::cerr << "[AudioManager] Error loading BGM: " << bgmFiles[index] << std::endl;
    } else {
        bgMusic.setLoop(true);
        // Only play if music is actually enabled
        if (settings.musicEnabled) {
            bgMusic.play();
        }
    }
    // Update volume immediately
    bgMusic.setVolume(settings.volume);
}

void AudioManager::updateMusicState() {
    bgMusic.setVolume(settings.volume);

    if (settings.musicEnabled) {
        if (bgMusic.getStatus() != sf::Music::Playing) {
            bgMusic.play();
        }
    } else {
        if (bgMusic.getStatus() == sf::Music::Playing) {
            bgMusic.stop();
        }
    }
}

std::string AudioManager::getCurrentTrackName() const {
    if (settings.bgmIndex >= 0 && settings.bgmIndex < bgmNames.size()) {
        return bgmNames[settings.bgmIndex];
    }
    return "Unknown";
}

int AudioManager::getTrackCount() const {
    return bgmFiles.size();
}