#pragma once
#include "Definitions.h"
#include "AudioManager.h"
#include "Board.h" // Include your logic engine
#include <vector>
#include <string>

class GoGame {
private:
    // The "Brain" of the game
    GoEngine engine;

    AudioManager& audio;

public:
    GoGame(AudioManager& am);

    // --- Getters for UI ---
    Stone getCurrentPlayer() const;
    Stone getStoneAt(int x, int y) const;

    // --- Actions ---
    // Returns true if move was valid
    bool placeStone(int x, int y);
    bool passTurn();

    bool undo();
    bool redo(); // Note: Your engine needs to support redo, or we rely on engine's history

    void resetGame();

    // --- Persistence ---
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

    // --- NEW: Get Score Info ---
    // Returns {Black Score, White Score}
    std::pair<float, float> getScore();
};