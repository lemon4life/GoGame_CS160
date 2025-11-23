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
    const auto& b = engine.getBoard();

    // 3. Map Coordinates: UI(x, y) -> Engine(x+1, y+1)
    Player p = b[x + 1][y + 1];

    return playerToStone(p);
}

bool GoGame::passTurn() {
    bool isGameOver = engine.pass_move();
    return isGameOver;
}

int GoGame::placeStone(int x, int y) {
    // 1. Convert UI Coordinates (0-based) to Engine Coordinates (1-based)
    int engineX = x + 1;
    int engineY = y + 1;

    bool captured;

    bool success = engine.make_move(engineX, engineY, captured);

    if (success) {
        if (captured) {
            audio.playCapture();
            return -1;
        } else {
            audio.playPlaceStone();
            return 1;
        }
    } else {
        audio.playError();
    }
    return 0;
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

bool GoGame::saveGame(const std::string& filename) {
    return engine.saveGame(filename);
}

bool GoGame::loadGame(const std::string& filename) {
    return engine.loadGame(filename);
}

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
    return engine.getDeadState();
}

std::vector<std::vector<bool>> GoGame::getValidMoves() const {
    return const_cast<GoEngine&>(engine).validMoves();
}