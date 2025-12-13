#include "GoGame.h"
#include "../include/KataGoRunner.h"
#include <bits/stdc++.h>
#include <iostream>
#include <sstream>

using namespace std;

Stone playerToStone(Player p) {
    if (p == Player::BLACK) return Stone::Black;
    if (p == Player::WHITE) return Stone::White;
    return Stone::Empty;
}

enum Difficulty { EASY, MEDIUM, HARD, NONE };

GoGame::GoGame(AudioManager& am)
    : audio(am), currentMode(GameMode::PVP), isAiThinking(false)
{
    engine.initialize_board(BOARD_SIZE);
}

void GoGame::setGameMode(GameMode mode) {
    currentMode = mode;
}

Stone GoGame::getCurrentPlayer() const { return playerToStone(engine.getCurrentPlayer()); }

Stone GoGame::getStoneAt(int x, int y) const {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return Stone::Empty;
    const auto& b = engine.getBoard();
    return playerToStone(b[x + 1][y + 1]);
}

bool GoGame::placeStone(int x, int y) {
    // If AI is thinking, block human input
    if (currentMode == GameMode::AI && isAiThinking) return false;

    int engineX = x + 1;
    int engineY = y + 1;
    bool captured = false;
    bool success = engine.make_move(engineX, engineY, captured);

    if (success) {
        if (captured) audio.playCapture();
        else audio.playPlaceStone();

        // If VS AI and human (Black) just played, trigger AI (White)
        if (currentMode == GameMode::AI && engine.getCurrentPlayer() == Player::WHITE) {
            isAiThinking = true;

            // Convert coordinate to GTP format (e.g., Q16)
            // This is complex. KataGoRunner::sendCommand handles sending.
            // But we need to tell KataGo the HUMAN move first.
            // Assuming KataGo tracks state internally? No, GTP is stateless usually unless we sync.
            // Actually, we usually send "play B Q16" then "genmove W".

            // For this snippet, I'll assume we call updateAI() in main loop to handle async processing
            // to avoid freezing the UI.
        }
    } else {
        audio.playError();
    }
    return success;
}

bool GoGame::isAIThinking() const {
    return isAiThinking;
}

bool GoGame::passTurn() { return engine.pass_move(); }
bool GoGame::undo() {
    if (engine.undo_step()) {
        audio.playPlaceStone();
        return true;
    }

    audio.playError();
    return false;
}
bool GoGame::redo() {
    if (engine.redo_step()) {
        audio.playPlaceStone();
        return true;
    }

    audio.playError();
    return false;
}
void GoGame::resetGame() { engine.initialize_board(BOARD_SIZE); }

Difficulty GoGame::getAIDifficulty() const {
    return currentDifficulty;
}

GameMode GoGame::getGameMode() const {
    return currentMode;
}

bool GoGame::saveGame(const std::string& f) {
    // Cast Enums to Int
    return engine.saveGame(f, (int)currentMode, (int)currentDifficulty);
}

bool GoGame::loadGame(const std::string& f) {
    int modeInt = 0;
    int diffInt = 0;

    if (engine.loadGame(f, modeInt, diffInt)) {
        // Convert Ints back to Enums
        currentMode = (modeInt == 1) ? GameMode::AI : GameMode::PVP;

        if (diffInt == 1) currentDifficulty = Difficulty::EASY;
        else if (diffInt == 2) currentDifficulty = Difficulty::MEDIUM;
        else if (diffInt == 3) currentDifficulty = Difficulty::HARD;
        else currentDifficulty = Difficulty::NONE;

        return true;
    }
    return false;
}

std::pair<float, float> GoGame::getScore() { return engine.calculateScore(); }
void GoGame::toggleDeadStone(int x, int y) { engine.toggle_life_death(x + 1, y + 1); }
std::vector<std::vector<bool>> GoGame::getDeadStones() const {
    auto rawDead = const_cast<GoEngine*>(&engine)->getDeadState();
    std::vector<std::vector<bool>> uiDead(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
             if (i+1 < rawDead.size() && j+1 < rawDead[i+1].size())
                uiDead[i][j] = rawDead[i + 1][j + 1];
        }
    }
    return uiDead;
}
std::vector<std::vector<bool>> GoGame::getValidMoves() const {
    return const_cast<GoEngine&>(engine).validMoves();
}

void GoGame::initAI(Difficulty level) {
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

    setDifficulty(level);
    currentDifficulty = level;
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
    isAiThinking = true;
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
    isAiThinking = false;
}

void GoGame::setDifficulty(Difficulty level) {
    string visits;
    
    switch(level) {
        case EASY:
            // Very fast, "weaker" (still strong)
            visits = "3"; 
            break;
        case MEDIUM:
            // Good balance of speed and strength
            visits = "10"; 
            break;
        case HARD:
            // "100%" capability (within reasonable waiting time)
            visits = "100"; 
            break;
    }

    // "kata-set-param" is a special KataGo command to change config on the fly
    std::string command = "kata-set-param maxVisits " + visits;
    
    // Send it! (Ignore the response, it's usually just success)
    bot.sendCommand(command);
    
    std::cout << "Difficulty set to " << visits << " visits." << std::endl;
}