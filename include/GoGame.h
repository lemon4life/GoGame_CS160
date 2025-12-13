#pragma once
#include "Definitions.h"
#include "AudioManager.h"
#include "Board.h"
#include "KataGoRunner.h" // <--- Include Partner's Header
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

    void setDifficulty(Difficulty level);

public:


    GoGame(AudioManager& am);

    Stone getCurrentPlayer() const;
    Stone getStoneAt(int x, int y) const;

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

    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

    std::pair<float, float> getScore();
    void toggleDeadStone(int x, int y);
    std::vector<std::vector<bool>> getDeadStones() const;
    void runHeuristic();
    void initAI(Difficulty level);

    void handleHumanMove(int x, int y, Player side);
    void doAITurn(Player side, int &x, int &y);
    std::vector<std::vector<bool>> getValidMoves() const;
};