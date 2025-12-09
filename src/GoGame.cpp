#include "GoGame.h"
#include "../include/KataGoRunner.h"
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

//Check for current player
Stone GoGame::getCurrentPlayer() const {
    return playerToStone(engine.getCurrentPlayer());
}

//Check for stone in intersection
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

//Pass the turn
bool GoGame::passTurn() {
    bool isGameOver = engine.pass_move();
    return isGameOver;
}

//Play the move
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

//Zobrist's Algorithm
void GoGame::runHeuristic() {
    engine.deadStoneHeuristic();
}

std::vector<std::vector<bool>> GoGame::getDeadStones() const {
    return engine.getDeadState();
}

std::vector<std::vector<bool>> GoGame::getValidMoves() const {
    return const_cast<GoEngine&>(engine).validMoves();
}

void GoGame::initAI() {
    // 2. Start the process. 
    // IMPORTANT: These paths are relative to where the .exe runs (your project root usually).
    bool isRunning = bot.startEngine("./AI/katago.exe", "./AI/model.bin.gz", "./AI/cpu_config.cfg");

    if (!isRunning) {
        std::cerr << "CRITICAL ERROR: AI failed to start. Check file paths!" << std::endl;
        return;
    }

    // 3. The "Handshake" (Basic Setup)
    // You MUST tell the AI the rules before playing.
    
    // Check if it's alive
    std::cout << "AI says: " << bot.sendCommand("name") << std::endl; 
    
    // Set Board Size (Standard is 19)
    bot.sendCommand("boardsize 19");
    
    // Set Komi (Points given to white, usually 6.5 or 7.5)
    bot.sendCommand("komi 6.5");
    
    // Clear the board (Good practice to ensure a clean state)
    bot.sendCommand("clear_board");
}

string GoGame::convertToGTP(int x, int y){
    const char cols[] = "ABCDEFGHJKLMNOPQRST";
    if(x < 0 || x > 18 || y < 0 || y > 18) return "pass";
    string coord = "";
    coord += cols[x];
    coord += to_string(y);
    return coord;
}

void GoGame::convertFromGTP(std::string gtp, int &x, int &y) {
    if (gtp.length() < 2) return;

    char colChar = std::toupper(gtp[0]);
    
    // Handle Column (Letter)
    if (colChar >= 'A' && colChar <= 'H') {
        x = colChar - 'A';
    } else if (colChar >= 'J' && colChar <= 'T') {
        x = colChar - 'A' - 1; // Adjust for skipped 'I'
    } else {
        x = 0; // Error
    }

    // Handle Row (Number)
    // The part after the letter is the number (e.g. "16" in "Q16")
    try {
        int rowNum = std::stoi(gtp.substr(1));
        y = 19 - rowNum; // Convert bottom-up (GTP) to top-down (Array/SFML)
    } catch (...) {
        y = 0;
    }
}

void GoGame::handleHumanMove(int x, int y, Player side) {
    std::string coord = convertToGTP(x, y); 
    placeStone(x, y);
    if(side == Player::BLACK) bot.sendCommand("play B " + coord);
    else bot.sendCommand("play W" + coord);
}

void GoGame::doAITurn(Player side, int &x, int &y) {
    // 1. Ask for a move (White)
    std::string response; 
    if(side == Player::WHITE) response = bot.sendCommand("genmove W");
    else response = bot.sendCommand("genmove B");
    
    // Response will be something like "= D16\n\n" or "= PASS\n\n"
    
    // 2. Clean the response (Remove "= " and newlines)
    // You can add a helper in KataGoRunner to do this, or do it here:
    std::string move = response.substr(2); // Skip "= "
    // Remove newlines
    move.erase(std::remove(move.begin(), move.end(), '\n'), move.end()); 
    move.erase(std::remove(move.begin(), move.end(), ' '), move.end());

    if (move == "PASS") {
        x = -1; y = -1;
        return;
    }

    if(move == "resign"){
        x = -2; y = -2;
        return;
    }
    // 3. Convert "D16" back to (x, y) integers
    int aiX, aiY;
    convertFromGTP(move, aiX, aiY);

    x = aiX; y = aiY;
}