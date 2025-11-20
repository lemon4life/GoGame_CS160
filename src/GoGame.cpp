#include "GoGame.h"
#include <iostream>

// --- Helper to convert Engine Enum to UI Enum ---
Stone playerToStone(Player p) {
    if (p == Player::BLACK) return Stone::Black;
    if (p == Player::WHITE) return Stone::White;
    return Stone::Empty;
}

// Constructor
GoGame::GoGame(AudioManager& am) : audio(am) {
    // Initialize the engine (19x19)
    engine.initialize_board(BOARD_SIZE);
}

// Get Current Player
Stone GoGame::getCurrentPlayer() const {
    return playerToStone(engine.getCurrentPlayer());
}

// Get Stone at Position (UI Coordinates 0-18)
Stone GoGame::getStoneAt(int x, int y) const {
    // Engine uses 1-based indexing (1-19), UI uses 0-based (0-18)
    // We need to map UI(x,y) -> Engine(x+1, y+1)

    // Get the raw board from engine
    // Note: Engine board is vector<vector<Player>>
    const auto& engineBoard = engine.getBoard();

    // Safety check for bounds
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return Stone::Empty;

    // Map 0-based to 1-based
    Player p = engineBoard[x + 1][y + 1];
    return playerToStone(p);
}

// Place Stone (UI Coordinates 0-18)
bool GoGame::placeStone(int x, int y) {
    // 1. Convert to Engine Coordinates (1-based)
    int engineX = x + 1;
    int engineY = y + 1;

    // 2. Ask Engine to make the move
    // make_move returns true if successful, false if invalid (occupied, suicide, ko)
    bool success = engine.make_move(engineX, engineY);

    // 3. If successful, play sound and return true
    if (success) {
        audio.playPlaceStone();
        return true;
    }

    return false;
}

// Undo Move
bool GoGame::undo() {
    // You might want to add a check in Engine if undo is possible (history not empty)
    engine.undo_step();
    return true;
}

// Redo Move
bool GoGame::redo() {
    engine.redo_step();
    return true;
}

// Reset Game
void GoGame::resetGame() {
    engine.initialize_board(BOARD_SIZE);
}

// Save Game
bool GoGame::saveGame(const std::string& filename) {
    // Delegate directly to engine
    return engine.saveGame(filename);
}

// Load Game
bool GoGame::loadGame(const std::string& filename) {
    // Delegate directly to engine
    return engine.loadGame(filename);
}