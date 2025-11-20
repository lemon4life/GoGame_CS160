#include "GoGame.h"
#include "Board.h"
#include <fstream>

// Constructor
GoGame::GoGame(AudioManager& am) : current_player(Stone::Black), audio(am) {
    board.resize(BOARD_SIZE, std::vector<Stone>(BOARD_SIZE, Stone::Empty));
}

// Get Current Player
Stone GoGame::getCurrentPlayer() const {
    return current_player;
}

// Get Stone at Position
Stone GoGame::getStoneAt(int x, int y) const {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        return Stone::Empty;
    return board[y][x];
}

// Place Stone
bool GoGame::placeStone(int x, int y) {
    // 1. Check validity
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE || board[y][x] != Stone::Empty)
        return false;

    // 2. Record the move for Undo
    Move move = {x, y, current_player};
    undoHistory.push_back(move);

    // 3. Clear Redo history (cannot redo after making a new move)
    redoHistory.clear();

    // 4. Place stone and switch turn
    board[y][x] = current_player;
    current_player = (current_player == Stone::Black ? Stone::White : Stone::Black);

    // 5. Play Sound
    audio.playPlaceStone();

    return true;
}

// Undo Move
bool GoGame::undo() {
    if (undoHistory.empty()) return false;

    // Get last move
    Move lastMove = undoHistory.back();
    undoHistory.pop_back();

    // Remove stone from board
    board[lastMove.y][lastMove.x] = Stone::Empty;

    // Add to Redo stack
    redoHistory.push_back(lastMove);

    // Switch turn back to the person who made that move
    current_player = lastMove.player;
    return true;
}

// Redo Move
bool GoGame::redo() {
    if (redoHistory.empty()) return false;

    // Get the move to redo
    Move nextMove = redoHistory.back();
    redoHistory.pop_back();

    // Place stone back on board
    board[nextMove.y][nextMove.x] = nextMove.player;

    // Add back to Undo stack
    undoHistory.push_back(nextMove);

    // Switch turn to the next player
    current_player = (nextMove.player == Stone::Black ? Stone::White : Stone::Black);
    return true;
}

// Reset Game
void GoGame::resetGame() {
    for (auto& row : board) {
        std::fill(row.begin(), row.end(), Stone::Empty);
    }
    current_player = Stone::Black;
    undoHistory.clear();
    redoHistory.clear();
}

// Save Game
bool GoGame::saveGame(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false;

    // Save current player
    outFile << (current_player == Stone::Black ? 1 : 2) << "\n";

    // Save Board
    for (const auto& row : board) {
        for (const auto& cell : row) {
            int val = 0;
            if (cell == Stone::Black) val = 1;
            if (cell == Stone::White) val = 2;
            outFile << val << " ";
        }
        outFile << "\n";
    }
    return true;
}

// Load Game
bool GoGame::loadGame(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false;

    int playerVal;
    inFile >> playerVal;
    current_player = (playerVal == 1 ? Stone::Black : Stone::White);

    // Clear history on load to prevent inconsistencies
    undoHistory.clear();
    redoHistory.clear();

    for (int y = 0; y < BOARD_SIZE; ++y) {
        for (int x = 0; x < BOARD_SIZE; ++x) {
            int val;
            inFile >> val;
            if (val == 1) board[y][x] = Stone::Black;
            else if (val == 2) board[y][x] = Stone::White;
            else board[y][x] = Stone::Empty;
        }
    }
    return true;
}