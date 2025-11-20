#include "../../include/board.h"

using namespace std;
//--------PRIVATE FUNCTION DEFINITIONS----------
//Store and cut branches
void GoEngine::store_to_history(){
    history.board.push_back(board);
    history.cur_hash.push_back(cur_hash);
    history.consecutivePass.push_back(consecutivePass);
    superkoMap[cur_hash]++;
}

void GoEngine::delete_branch(int move){
    while(history.board.size() > move){
        history.board.pop_back();
        history.cur_hash.pop_back();
        history.consecutivePass.pop_back();
    }
}

//Automatic State Reassessment
pair<int, int> GoEngine::spread(int x_coor, int y_coor, int dir){
    int switch_row[4] = {1, -1, 0, 0}, switch_col[4] = {0, 0, 1, -1};

    return {x_coor + switch_row[dir], y_coor + switch_col[dir]};
}

void GoEngine::delete_clump(int x_coor, int y_coor){
    queue< pair<int, int> > q;
    Player ini = board[x_coor][y_coor];
    q.push({x_coor, y_coor});

    while(!q.empty()){
        auto [x, y] = q.front();
        //cout << x << " " << y << endl;
        q.pop();
        board[x][y] = Player::NONE;
        for(int i = 0; i < 4; i++){
            auto [xn, yn] = spread(x, y, i);
            if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
                if(board[xn][yn] == ini) q.push({xn, yn});
            }
        }
    }
}

bool GoEngine::check_survivability(int x_coor, int y_coor){
    queue< pair<int, int> > q;
    q.push({x_coor, y_coor});
    int lib_count = 0; Player ini = board[x_coor][y_coor];
    vector<vector<bool>> vir_board(boardSize+1, vector<bool>(boardSize+1, 0));
    while(!q.empty()){
        auto [x, y] = q.front();
        //cout << x << " " << y << endl;
        vir_board[x][y] = 1;
        q.pop();

        for(int i = 0; i < 4; i++){
            auto[xn, yn] = spread(x, y, i);
            if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
                if(board[xn][yn] == Player::NONE) lib_count++;
                else if(board[xn][yn] == ini && !vir_board[xn][yn]) q.push({xn, yn});
            }
        }
    }

    if(!lib_count) {
        delete_clump(x_coor, y_coor);
        return 0;
    }
    return 1;
}

bool GoEngine::reassest_board_state(int x, int y){
    for(int i = 0; i < 4; i++){
        auto [xn, yn] = spread(x, y, i);
        if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
            if(board[xn][yn] != Player::NONE && board[xn][yn] != board[x][y]) {
                check_survivability(xn, yn);
            }
        }
    }
    if(!check_survivability(x, y)) return false;
    cur_hash = calcZobrist();
    return checkSuperko();
}

//Zobrist Hash and SuperKo construction

void GoEngine::initZobrist(int brd) {
    mt19937_64 rng(123456); // fixed seed for reproducibility
    uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    zobristTable = vector<vector<vector<uint64_t>>>(brd+1, vector<vector<uint64_t>>(brd+1, vector<uint64_t>(2, 0)));
    for (int x = 1; x <= brd; ++x) {
        for (int y = 1; y <= brd; ++y) {
            for (int c = 0; c < 2; ++c) {
                zobristTable[x][y][c] = dist(rng);
            }
        }
    }
}

uint64_t GoEngine::calcZobrist(){
    uint64_t val = 0;
    for(int i = 1; i <= boardSize; i++){
        for(int j = 1; j <= boardSize; j++) if(board[i][j] != Player::NONE) val ^= zobristTable[i][j][convert_numeral(board[i][j]) - 1];
    }

    return val;
}

bool GoEngine::checkSuperko(){
    if(superkoMap.count(cur_hash)) return false;
    return true;
}

int GoEngine::convert_numeral(Player a){
    if(a == Player::NONE) return 0;
    if(a == Player::WHITE) return 1;
    return 2;
}
void GoEngine::switchPlayer(){
    if(currentPlayer == Player::BLACK) currentPlayer = Player::WHITE;
    if(currentPlayer == Player::WHITE) currentPlayer = Player::BLACK;
}
void GoEngine::initialize_board(int Size){
    boardSize = Size; cur_move = 0;
    board.clear(); board.assign(Size+1, vector<Player>(Size+1, Player::NONE));
    zobristTable.clear(); zobristTable.assign(Size+1, vector<vector<uint64_t>>(Size+1, vector<uint64_t>(2, 0)));

    superkoMap.clear();
    cur_hash = 0;
    history = {};
    consecutivePass = 0;
    store_to_history();
    initZobrist(Size);
}

bool GoEngine::make_move(int x, int y){
    cur_move++;
    switchPlayer();
    if(board[x][y] != Player::NONE) {
        cur_move--;
        return false;
    }
    board[x][y] = currentPlayer;
    if(!reassest_board_state(x, y)){
        undo_step();
        //cout << "Move: " << cur_move << endl;
        //cout << "Again!" << endl;
        return false;
    }
    delete_branch(cur_move);
    store_to_history();
    consecutivePass = 0;
    return true;
}

void GoEngine::pass_move(){
    cur_move++;
    delete_branch(cur_move);
    store_to_history();
    consecutivePass++;
    if(consecutivePass == 2);
}

void GoEngine::undo_step(){
    cur_move--;
    switchPlayer();
    //cout << cur_move << endl;
    superkoMap.erase(cur_hash);
    cur_hash = history.cur_hash[cur_move];
    board = history.board[cur_move];
    consecutivePass = history.consecutivePass[cur_move];
}

void GoEngine::redo_step(){
    cur_move++;
    cur_hash = history.cur_hash[cur_move]; superkoMap[cur_hash]++;
    board = history.board[cur_move];
    consecutivePass = history.consecutivePass[cur_move];
}