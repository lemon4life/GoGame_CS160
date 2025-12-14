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
    } else {
        audio.playError();
    }
    return success;
}

bool GoGame::isAIThinking() const {
    return isAiThinking;
}

// Updated passTurn to handle AI synchronization
bool GoGame::passTurn() {
    if (currentMode == GameMode::AI) {
        // If it's Human's turn (Black), we must tell the bot we passed.
        // If it's AI's turn (White), the AI just generated the pass itself, so we don't send it back.
        if (engine.getCurrentPlayer() == Player::BLACK) {
            bot.sendCommand("play B pass");
        }
    }
    return engine.pass_move();
}

bool GoGame::undo() {
    if (engine.undo_step()) {
        audio.playPlaceStone();
        if (currentMode == GameMode::AI) {
            bot.sendCommand("undo");
            if (engine.undo_step()){
                audio.playPlaceStone();
                bot.sendCommand("undo");
            }
        }
        return true;
    }

    audio.playError();
    return false;
}

bool GoGame::redo() {
    int x = 0, y = 0; string player;
    if (engine.redo_step(x, y, player)) {
        audio.playPlaceStone();

        if (currentMode == GameMode::AI) {
            std::string coord = convertToGTP(x, y);
            bot.sendCommand("play " + player + " " + coord);
            std::cout << "play " << player << " " << coord << std::endl;
            if (engine.redo_step(x, y, player)) {
                audio.playPlaceStone();
                coord = convertToGTP(x, y);
                bot.sendCommand("play " + player + " " + coord);
                std::cout << "play " << player << " " << coord << std::endl;
            }
        }
        return true;
    }

    audio.playError();
    return false;
}

void GoGame::resetGame() {
    engine.initialize_board(BOARD_SIZE);
    if (currentMode == GameMode::AI) {
        bot.sendCommand("clear_board");
    }
}

Difficulty GoGame::getAIDifficulty() const {
    return currentDifficulty;
}

GameMode GoGame::getGameMode() const {
    return currentMode;
}

bool GoGame::saveGame(const std::string& f, const std::string& mode) {
    return engine.saveGame(f, mode);
}

bool GoGame::loadGame(const std::string& f, std::string& mode) {
    if (engine.loadGame(f, mode)) {
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

void GoGame::runHeuristic() {
    engine.deadStoneHeuristic();
}

void GoGame::initAI(std::string exe, std::string model, std::string cfg) {
    if (currentMode == GameMode::AI) {
        if (!bot.startEngine(exe, model, cfg)) {
            std::cerr << "Failed to start AI Engine!" << std::endl;
            currentMode = GameMode::PVP;
            return;
        }
        // Initialize Bot Settings
        bot.sendCommand("boardsize 19");
        bot.sendCommand("komi 6.5");
        bot.sendCommand("clear_board");
        // Default difficulty
        setDifficulty(Difficulty::MEDIUM);
    }
}

string GoGame::convertToGTP(int x, int y){
    const char cols[] = "ABCDEFGHJKLMNOPQRST";
    if(x < 0 || x > 18 || y < 0 || y > 18) return "pass";
    string coord = "";
    coord += cols[x];
    coord += to_string(19 - y);
    return coord;
}

void GoGame::convertFromGTP(std::string gtp, int &x, int &y) {
    std::string command = gtp;
    for (char &c : command) c = std::toupper(c);

    // --- FIX STARTS HERE ---
    // Handle PASS (-1, -1)
    if (command == "PASS") {
        x = -1;
        y = -1;
        std::cout << "AI Action: PASS" << std::endl;
        return; // Stop here! Don't try to parse coordinates.
    }

    // Handle RESIGN (-2, -2)
    if (command == "RESIGN") {
        x = -2;
        y = -2;
        std::cout << "AI Action: RESIGN" << std::endl;
        return; // Stop here!
    }
    // --- FIX ENDS HERE ---

    // The rest of your original logic follows...
    if (gtp.length() < 2) return;

    char colChar = std::toupper(gtp[0]);

    if (colChar >= 'A' && colChar <= 'H') {
        x = colChar - 'A';
    } else if (colChar >= 'J' && colChar <= 'T') {
        x = colChar - 'A' - 1;
    } else {
        x = 0;
    }

    try {
        int rowNum = std::stoi(gtp.substr(1));
        y = 19 - rowNum;
    } catch (...) {
        y = 0;
    }
    std::cout << x << " " << y << std::endl;
}

// simplified handleHumanMove
void GoGame::handleHumanMove(int x, int y) {
    std::string coord = convertToGTP(x, y);
    bool success = placeStone(x, y);

    // Only send if move was valid and we are in AI mode
    if(success && currentMode == GameMode::AI) {
        bot.sendCommand("play B " + coord);
    }
}

// simplified doAITurn
void GoGame::doAITurn(int &x, int &y) {
    isAiThinking = true;

    // AI is always White
    std::string response = bot.sendCommand("genmove W");

    std::string move = response.substr(2); // Skip "= "
    move.erase(std::remove(move.begin(), move.end(), '\n'), move.end());
    move.erase(std::remove(move.begin(), move.end(), ' '), move.end());

    if (move == "PASS") {
        x = -1; y = -1;
    } else if (move == "resign") {
        x = -2; y = -2;
    } else {
        convertFromGTP(move, x, y);
    }

    isAiThinking = false;
}

void GoGame::setDifficulty(Difficulty level) {
    string visits;
    switch(level) {
        case Difficulty::EASY: visits = "3"; break;
        case Difficulty::MEDIUM: visits = "25"; break;
        case Difficulty::HARD: visits = "100"; break;
        default: visits = "10"; break;
    }
    bot.sendCommand("kata-set-param maxVisits " + visits);
    currentDifficulty = level;
    std::cout << "Difficulty set to " << visits << " visits." << std::endl;
}

