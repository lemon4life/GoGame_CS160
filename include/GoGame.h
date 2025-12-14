#pragma once
#include "Definitions.h"
#include "AudioManager.h"
#include "Board.h"
#include "KataGoRunner.h"
#include <vector>
#include <string>
#include <utility>

using namespace std;

class GoGame {
private:
    GoEngine engine;
    AudioManager& audio;

    string convertToGTP(int x, int y);
    void convertFromGTP(std::string gtp, int &x, int &y);

    // --- AI Components ---
    KataGoRunner bot;
    GameMode currentMode;
    bool isAiThinking;
    Difficulty currentDifficulty;

    int getTerritoryOwner(int x, int y);

public:
    GoGame(AudioManager& am);

    Stone getCurrentPlayer() const;
    Stone getStoneAt(int x, int y) const;
    void setDifficulty(Difficulty level);

    // Actions
    bool placeStone(int x, int y);
    bool passTurn();

    // AI Specific
    void setGameMode(GameMode mode);
    bool isAIThinking() const;

    bool undo();
    bool redo();
    void resetGame();

    Difficulty getAIDifficulty() const;
    GameMode getGameMode() const;

    bool saveGame(const std::string& filename, Difficulty mode);
    bool loadGame(const std::string& filename, Difficulty& mode);

    std::pair<float, float> getScore();
    void toggleDeadStone(int x, int y);
    std::vector<std::vector<bool>> getDeadStones() const;
    void runHeuristic();
    void initAI(std::string exe, std::string model, std::string cfg);

    // Updated Signatures
    void handleHumanMove(int x, int y);
    void doAITurn(int &x, int &y);

    std::vector<std::vector<bool>> getValidMoves() const;

    std::pair<float, float> calculateExactScore();
};