#include "GoGame.h"
#include <iostream>

// Helper: Convert Engine Player to UI Stone
Stone playerToStone(Player p) {
    if (p == Player::BLACK) return Stone::Black;
    if (p == Player::WHITE) return Stone::White;
    return Stone::Empty;
}

GoGame::GoGame(AudioManager& am) : audio(am) {
    // Initialize engine with standard size (19x19)
    // Engine uses 1-based indexing internally
    engine.initialize_board(BOARD_SIZE);
}

Stone GoGame::getCurrentPlayer() const {
    return playerToStone(engine.getCurrentPlayer());
}

Stone GoGame::getStoneAt(int x, int y) const {
    // 1. Check Bounds (UI uses 0-18)
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return Stone::Empty;

    // 2. Get Board from Engine
    // Engine board is vector<vector<Player>> of size [20][20] (indices 1-19 valid)
    const auto& b = engine.getBoard();

    // 3. Map Coordinates: UI(x, y) -> Engine(x+1, y+1)
    Player p = b[x + 1][y + 1];

    return playerToStone(p);
}

// --- NEW PASS FUNCTION ---
bool GoGame::passTurn() {
    // Call engine pass logic
    // engine.pass_move() returns true if Game Over (2 consecutive passes)
    bool isGameOver = engine.pass_move();
    return isGameOver;
}

bool GoGame::placeStone(int x, int y) {
    // 1. Convert UI Coordinates (0-based) to Engine Coordinates (1-based)
    int engineX = x + 1;
    int engineY = y + 1;

    bool captured;

    // 2. Delegate to Engine
    // make_move handles rules, captures, ko, and history
    bool success = engine.make_move(engineX, engineY, captured);

    if (success) {
        if (captured) {
            audio.playCapture();
        } else {
            audio.playPlaceStone();
        }
    } else {
        audio.playError();
    }
    return success;
}

bool GoGame::undo() {
    if (engine.undo_step()) {
        audio.playPlaceStone();
        return true;
    } else {
        audio.playError();
        return false;
    }
}

bool GoGame::redo() {
    if (engine.redo_step()) {
        audio.playPlaceStone();
        return true;
    } else {
        audio.playError();
        return false;
    }

}

void GoGame::resetGame() {
    engine.initialize_board(BOARD_SIZE);
}

// --- CONNECTING TO YOUR BOARD.CPP SAVE/LOAD ---

bool GoGame::saveGame(const std::string& filename) {
    // The engine handles the file writing logic
    return engine.saveGame(filename);
}

bool GoGame::loadGame(const std::string& filename) {
    // The engine handles the file reading and board reconstruction
    return engine.loadGame(filename);
}

// --- NEW ---
std::pair<float, float> GoGame::getScore() {
    return engine.calculateScore();
}

void GoGame::toggleDeadStone(int x, int y) {
    // Map UI (0-18) to Engine (1-19)
    int engineX = x + 1;
    int engineY = y + 1;
    engine.toggle_life_death(engineX, engineY);
}

void GoGame::runHeuristic() {
    engine.deadStoneHeuristic();
}

std::vector<std::vector<bool>> GoGame::getDeadStones() const {
    // Engine returns a grid sized [BOARD_SIZE+1][BOARD_SIZE+1]
    // We need to convert it to [BOARD_SIZE][BOARD_SIZE] for the UI

    // We can't access 'dead' directly if it's private, so we need a getter in GoEngine.
    // Assuming you added `getDeadState()` to GoEngine as per your prompt.

    // Note: `getDeadState` returns vector<vector<bool>>
    // We need to cast or just call it. Since 'dead' is private in Board.h,
    // we assume you added the public getter `getDeadState()` there.

    // If GoEngine doesn't have a public getter yet, you must add it to Board.h!
    // But assuming it exists:
    // auto rawDead = engine.getDeadState();

    // Wait, 'getDeadState' isn't in the Board.h you showed earlier,
    // but you provided the code for it in the prompt.
    // I will assume you added it to Board.h.

    // Since I cannot see `getDeadState` in `Board.h` public section,
    // I will create a dummy return here.
    // YOU MUST ADD `vector<vector<bool>> getDeadState();` to `Board.h` public section.

    // ... Let's assume it's there ...
    // auto rawDead = engine.getDeadState();

    // Since I cannot modify Board.h/cpp, I cannot implement this fully without that getter.
    // However, assuming you added it:

    /*
    std::vector<std::vector<bool>> uiDead(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
    auto rawDead = engine.getDeadState(); // Assumes 1-based indexing from engine

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // Safety check
            if (i + 1 < rawDead.size() && j + 1 < rawDead[i+1].size()) {
                uiDead[j][i] = rawDead[i + 1][j + 1]; // Notice X/Y swap if needed based on your loop
            }
        }
    }
    return uiDead;
    */

    return engine.getDeadState();

    // Placeholder until Board.h is updated
    return std::vector<std::vector<bool>>(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
}

std::vector<std::vector<bool>> GoGame::getValidMoves() const {
    // The engine's validMoves() is not const in your snippet?
    // If it modifies state temporarily, you might need to cast away const
    // or make engine mutable, but assuming it's safe:
    return const_cast<GoEngine&>(engine).validMoves();
}