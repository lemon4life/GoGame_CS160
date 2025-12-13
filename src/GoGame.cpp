#include "GoGame.h"
#include <iostream>
#include <sstream>

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

void GoGame::initAI(std::string exe, std::string model, std::string cfg) {
    if (currentMode == GameMode::AI) {
        if (!aiRunner.startEngine(exe, model, cfg)) {
            std::cerr << "Failed to start AI Engine!" << std::endl;
            // Fallback to PvP or show error?
            currentMode = GameMode::PVP;
        }
    }
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

void GoGame::updateAI() {
    if (currentMode == GameMode::AI && isAiThinking) {
        // In a real implementation, you'd run this in a thread or check a future.
        // Since KataGoRunner uses pipes (blocking ReadFile in sendCommand),
        // calling this here WILL FREEZE the UI for 1-5 seconds.
        // Ideally, move this to a separate thread.

        // 1. Sync Logic: Send "genmove W"
        // (Assuming we already synced the board or KataGo tracks it.
        //  A robust implementation sends "play B <coord>" every time human moves.
        //  For simplicity here, we just ask for a move.)

        std::string response = aiRunner.sendCommand("genmove white");

        // 2. Parse response (e.g., "= Q16")
        // We need a helper to convert GTP coordinate "Q16" back to (x, y)
        // This is missing in your provided files, so I'll sketch it.

        // ... Parsing Logic ...
        // int aiX = ...; int aiY = ...;

        // 3. Apply Move
        // bool captured = false;
        // engine.make_move(aiX, aiY, captured);
        // audio.playPlaceStone();

        isAiThinking = false;
    }
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
void GoGame::setAIDifficulty(AIDifficulty diff) {
    currentDifficulty = diff;
}

AIDifficulty GoGame::getAIDifficulty() const {
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

        if (diffInt == 1) currentDifficulty = AIDifficulty::Easy;
        else if (diffInt == 2) currentDifficulty = AIDifficulty::Medium;
        else if (diffInt == 3) currentDifficulty = AIDifficulty::Hard;
        else currentDifficulty = AIDifficulty::None;

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
    return const_cast<GoEngine*>(&engine)->validMoves();
}