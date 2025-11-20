#include "Board.h"
#include <queue>
#include <random>
#include <iostream> // For std::cout debugging if needed

using namespace std;

//--------PRIVATE FUNCTION DEFINITIONS----------

// Store and cut branches
void GoEngine::store_to_history() {
    history.board.push_back(board);
    history.cur_hash.push_back(cur_hash);
    history.consecutivePass.push_back(consecutivePass);
    superkoMap[cur_hash]++;
}

void GoEngine::delete_branch(int move) {
    // Ensure we don't pop if history is smaller than move index
    while (history.board.size() > (size_t)move) {
        history.board.pop_back();
        history.cur_hash.pop_back();
        history.consecutivePass.pop_back();
    }
}

// Automatic State Reassessment
pair<int, int> GoEngine::spread(int x_coor, int y_coor, int dir) {
    int switch_row[4] = { 1, -1, 0, 0 }, switch_col[4] = { 0, 0, 1, -1 };
    return { x_coor + switch_row[dir], y_coor + switch_col[dir] };
}

void GoEngine::delete_clump(int x_coor, int y_coor) {
    queue< pair<int, int> > q;
    Player ini = board[x_coor][y_coor];
    q.push({ x_coor, y_coor });

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        board[x][y] = Player::NONE;
        for (int i = 0; i < 4; i++) {
            auto [xn, yn] = spread(x, y, i);
            if (xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize) {
                if (board[xn][yn] == ini) q.push({ xn, yn });
            }
        }
    }
}

bool GoEngine::check_survivability(int x_coor, int y_coor) {
    queue< pair<int, int> > q;
    q.push({ x_coor, y_coor });
    int lib_count = 0;
    Player ini = board[x_coor][y_coor];
    vector<vector<bool>> vir_board(boardSize + 1, vector<bool>(boardSize + 1, 0));

    while (!q.empty()) {
        auto [x, y] = q.front();
        vir_board[x][y] = 1;
        q.pop();

        for (int i = 0; i < 4; i++) {
            auto [xn, yn] = spread(x, y, i);
            if (xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize) {
                if (board[xn][yn] == Player::NONE) lib_count++;
                else if (board[xn][yn] == ini && !vir_board[xn][yn]) q.push({ xn, yn });
            }
        }
    }

    if (!lib_count) {
        delete_clump(x_coor, y_coor);
        return false; // Died
    }
    return true; // Survived
}

bool GoEngine::reassest_board_state(int x, int y) {
    // Check neighbors first (did we capture them?)
    for (int i = 0; i < 4; i++) {
        auto [xn, yn] = spread(x, y, i);
        if (xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize) {
            if (board[xn][yn] != Player::NONE && board[xn][yn] != board[x][y]) {
                check_survivability(xn, yn);
            }
        }
    }

    // Check self (did we commit suicide?)
    if (!check_survivability(x, y)) return false;

    cur_hash = calcZobrist();
    return checkSuperko();
}

// Zobrist Hash and SuperKo construction
void GoEngine::initZobrist(int brd) {
    mt19937_64 rng(123456);
    uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    // Resize 3D vector
    zobristTable = vector<vector<vector<uint64_t>>>(brd + 1, vector<vector<uint64_t>>(brd + 1, vector<uint64_t>(2, 0)));

    for (int x = 1; x <= brd; ++x) {
        for (int y = 1; y <= brd; ++y) {
            for (int c = 0; c < 2; ++c) {
                zobristTable[x][y][c] = dist(rng);
            }
        }
    }
}

uint64_t GoEngine::calcZobrist() {
    uint64_t val = 0;
    for (int i = 1; i <= boardSize; i++) {
        for (int j = 1; j <= boardSize; j++)
            if (board[i][j] != Player::NONE)
                val ^= zobristTable[i][j][convert_numeral(board[i][j]) - 1];
    }
    return val;
}

bool GoEngine::checkSuperko() {
    if (superkoMap.count(cur_hash)) return false;
    return true;
}

int GoEngine::convert_numeral(Player a) {
    if (a == Player::NONE) return 0;
    if (a == Player::WHITE) return 1;
    return 2;
}

void GoEngine::switchPlayer() {
    if (currentPlayer == Player::BLACK) currentPlayer = Player::WHITE;
    else if (currentPlayer == Player::WHITE) currentPlayer = Player::BLACK;
}

void GoEngine::initialize_board(int Size) {
    boardSize = Size;
    cur_move = 0;

    // Resize board (Size + 1 for 1-based indexing)
    board.clear();
    board.assign(Size + 1, vector<Player>(Size + 1, Player::NONE));

    zobristTable.clear();
    // Re-init zobrist table in case size changed
    // (Optimization: Check if size changed before clearing)
    initZobrist(Size);

    superkoMap.clear();
    cur_hash = 0;

    // Reset history
    history = {};
    consecutivePass = 0;
    currentPlayer = Player::BLACK; // Default start

    store_to_history();
}

bool GoEngine::make_move(int x, int y) {
    cur_move++;

    // Note: In standard Go, Black moves first.
    // If your logic relies on switching player *before* placing, ensure currentPlayer is initialized correctly.
    // Assuming currentPlayer holds the player *about to move*:
    // board[x][y] = currentPlayer;
    // Then switch?
    // Your original code: switchPlayer(); ... board[x][y] = currentPlayer;
    // This implies 'currentPlayer' stores the *previous* player?
    // Let's stick to your logic:

    switchPlayer(); // Switch to the person making the move

    if (board[x][y] != Player::NONE) {
        switchPlayer(); // Revert
        cur_move--;
        return false;
    }

    board[x][y] = currentPlayer;

    if (!reassest_board_state(x, y)) {
        undo_step(); // This reverts the move logic
        return false;
    }

    delete_branch(cur_move);
    store_to_history();
    consecutivePass = 0;
    return true;
}

void GoEngine::pass_move() {
    cur_move++;
    switchPlayer(); // Pass transfers turn
    delete_branch(cur_move);
    store_to_history();
    consecutivePass++;
    // if(consecutivePass == 2) handleGameEnd();
}

void GoEngine::undo_step() {
    if (cur_move <= 0) return; // Safety
    cur_move--;
    switchPlayer();

    superkoMap.erase(cur_hash);

    // Restore state
    cur_hash = history.cur_hash[cur_move];
    board = history.board[cur_move];
    consecutivePass = history.consecutivePass[cur_move];
}

void GoEngine::redo_step() {
    if (cur_move >= history.board.size() - 1) return; // Safety

    cur_move++;
    switchPlayer();

    cur_hash = history.cur_hash[cur_move];
    superkoMap[cur_hash]++;
    board = history.board[cur_move];
    consecutivePass = history.consecutivePass[cur_move];
}

// Getters
Player GoEngine::getCurrentPlayer() const {
    return currentPlayer;
}

vector<vector<Player>> GoEngine::getBoard() const {
    return board;
}

// Implement Save/Load dummies if needed for linker,
// or move your Save/Load logic from previous steps here.
bool GoEngine::saveGame(const std::string& filename) { return true; }
bool GoEngine::loadGame(const std::string& filename) { return true; }
std::pair<float, float> GoEngine::calculateScore() const { return {0.0f, 0.0f}; }