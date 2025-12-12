#pragma once
#include "Definitions.h"
#include "AudioManager.h"
#include "Board.h"
#include "KataGoRunner.h" // <--- Include Partner's Header
#include <vector>
#include <string>
#include <utility>

class GoGame {
private:
    GoEngine engine;
    AudioManager& audio;

    // --- AI Components ---
    KataGoRunner aiRunner;
    GameMode currentMode;
    bool isAiThinking;
    AIDifficulty currentDifficulty;

public:
    GoGame(AudioManager& am);

    Stone getCurrentPlayer() const;
    Stone getStoneAt(int x, int y) const;

    // Actions
    bool placeStone(int x, int y);
    bool passTurn();

    // AI Specific
    void setGameMode(GameMode mode);
    void initAI(std::string exe, std::string model, std::string cfg);
    void updateAI(); // Call this in main loop to check for AI move
    bool isAIThinking() const;

    bool undo();
    bool redo();
    void resetGame();

    void setAIDifficulty(AIDifficulty diff);
    AIDifficulty getAIDifficulty() const;
    GameMode getGameMode() const;

    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

    std::pair<float, float> getScore();
    void toggleDeadStone(int x, int y);
    std::vector<std::vector<bool>> getDeadStones() const;
    std::vector<std::vector<bool>> getValidMoves() const;
};