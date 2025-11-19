#pragma once
#include "Definitions.h"
#include "AudioManager.h" // Assuming you created this
#include <vector>
#include <string>

class GoGame {
private:
    std::vector<std::vector<Stone>> board;
    Stone current_player;
    std::vector<Move> undoHistory;
    std::vector<Move> redoHistory;
    AudioManager& audio; // Reference

public:
    GoGame(AudioManager& am);
    Stone getCurrentPlayer() const;
    Stone getStoneAt(int x, int y) const;
    bool placeStone(int x, int y);
    bool undo();
    bool redo();
    void resetGame();
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
};