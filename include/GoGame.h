#pragma once
#include "Definitions.h"
#include "AudioManager.h"
#include "Board.h" // Include your logic engine
#include <vector>
#include <string>

class GoGame {
private:
    GoEngine engine;

    AudioManager& audio;

    string convertToGTP(int x, int y);
    void convertFromGTP(std::string gtp, int &x, int &y);
public:
    GoGame(AudioManager& am);

    // --- Getters for UI ---
    Stone getCurrentPlayer() const;
    Stone getStoneAt(int x, int y) const;

    // --- Actions ---
    // Returns true if move was valid
    int placeStone(int x, int y);
    bool passTurn();

    bool undo();
    bool redo();

    void resetGame();

    // --- Persistence ---
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

    // --- NEW: Get Score Info ---
    // Returns {Black Score, White Score}
    std::pair<float, float> getScore();
    std::vector<std::vector<bool>> getValidMoves() const;

    // Toggle life/death status of a stone at (x, y)
    void toggleDeadStone(int x, int y);

    // Get the grid of dead stones (true = dead, false = alive)
    // Returns 19x19 vector for UI rendering
    std::vector<std::vector<bool>> getDeadStones() const;
    void runHeuristic();

    KataGoRunner bot;
    void initAI();

    void handleHumanMove(int x, int y, Player side);
    void doAITurn(Player side, int &x, int &y);
};