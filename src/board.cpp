#include "../include/board.h"

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
        coorSaver.pop_back();
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


void GoEngine::clean_up_dead(){
    for(int i = 1; i <= boardSize; i++)
        for(int j = 1; j <= boardSize; j++) if(dead[i][j] == 1) board[i][j] = Player::NONE;
}



//--------PUBLIC FUNCTION DEFINITIONS----------

void GoEngine::initialize_board(int Size){
    boardSize = Size; cur_move = 0; currentPlayer = Player::WHITE;
    board.clear(); board.assign(Size+1, vector<Player>(Size+1, Player::NONE));
    zobristTable.clear(); zobristTable.assign(Size+1, vector<vector<uint64_t>>(Size+1, vector<uint64_t>(2, 0)));

    superkoMap.clear();
    cur_hash = 0;
    history = {};
    coorSaver.clear();
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
    coorSaver.push_back({x, y});
    consecutivePass = 0;
    return true;
}

bool GoEngine::pass_move(){
    cur_move++;
    delete_branch(cur_move);
    store_to_history();
    consecutivePass++;
    coorSaver.push_back({-1, -1});
    if(consecutivePass == 2) return true;
    return false;
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

Player GoEngine::getCurrentPlayer() const{
    return currentPlayer;
}

vector< vector<Player> > GoEngine::getBoard() const{
    return board;
}

void GoEngine::deadStoneHeuristic(){
    vector< vector<int> > influence(boardSize+1, vector<int>(boardSize+1, 0));
    for(int i = 1; i <= boardSize; i++){
        for(int j = 1; j <= boardSize; j++){
            if(convert_numeral(board[i][j]) == 2) influence[i][j] = 128;
            if(convert_numeral(board[i][j]) == 1) influence[i][j] = -128;
        }
    }

    for(int t = 0; t < 5; t++){
        vector<vector<int> > new_map = influence;
        for(int i = 1; i <= boardSize; i++){
            for(int j = 1; j <= boardSize; j++){
                int Binf = 0, Winf = 0;
                for(int k = 0; k < 4; k++){
                    auto [xn, yn] = spread(i, j, k);
                    if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
                        if(influence[xn][yn] > 0) Binf++;
                        else if(influence[xn][yn] < 0) Winf++;
                    }
                }
                if(Binf == 0) new_map[i][j] -= Winf;
                if(Winf == 0) new_map[i][j] += Binf;
            }
        }
        influence = new_map;
    }

    for(int t = 0; t < 21; t++){
        vector< vector<int> > new_map = influence;
        for(int i = 1; i <= boardSize; i++){
            for(int j = 1; j <= boardSize; j++){
                for(int k = 0; k < 4; k++){
                    auto [xn, yn] = spread(i, j, k);
                    if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
                        if(influence[i][j] > 0 && influence[xn][yn] <= 0) new_map[i][j]--;
                        if(influence[i][j] < 0 && influence[xn][yn] >= 0) new_map[i][j]++;
                    }
                }
            }
        }
        influence = new_map;
    }

    dead = vector< vector<bool> >(boardSize+1, vector<bool>(boardSize+1, 0));
    for(int i = 1; i <= boardSize; i++){
        for(int j = 1; j <= boardSize; j++) if((convert_numeral(board[i][j]) == 2 && influence[i][j] >= 0) || (convert_numeral(board[i][j]) == 1 && influence[i][j] <= 0)) dead[i][j] = 1;
    }
}

void GoEngine::toggle_life_death(int x, int y){
    dead[x][y] = dead[x][y] ^ 1;
}

pair<float, float> GoEngine::calculateScore(){
    clean_up_dead();
    vector< vector<bool> > checked(boardSize+1, vector<bool>(boardSize+1, 0));
    int Bpt = 0, Wpt = komi;
    for(int i = 1; i <= boardSize; i++){
        for(int j = 1; j <= boardSize; j++) if(checked[i][j] == 0){
            int cnt = 0; bool Badj = 0, Wadj = 0;
            queue<pair<int, int>> q;
            q.push({i, j});

            while(!q.empty()){
                auto [x, y] = q.front();
                q.pop();
                checked[x][y] = 1;
                cnt++;

                for(int k = 0; k < 4; k++){
                    auto [xn, yn] = spread(x, y, k);
                    if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize && !checked[xn][yn]){
                        if(board[xn][yn] == Player::BLACK) Badj = 1;
                        if(board[xn][yn] == Player::WHITE) Wadj = 1;
                        if(board[xn][yn] == board[i][j]) q.push({xn, yn});
                    }
                }
            }

            if(board[i][j] == Player::BLACK) Bpt += cnt;
            if(board[i][j] == Player::WHITE) Wpt += cnt;
            if(board[i][j] == Player::NONE){
                if(Badj == 0 && Wadj == 0) continue;
                if(!Wadj) Bpt += cnt;
                if(!Badj) Wpt += cnt; 
            }
        }
    }
    return {Bpt, Wpt};
}

bool GoEngine::saveGame(const std::string& filepath){
    ofstream outfile(filepath);

    if (!outfile.is_open()) {
        cerr << "Error: Could not open file for writing: " << filepath << endl;
        return false;
    }

    outfile << boardSize << endl;
    outfile << komi << endl;
    outfile << cur_move << endl;
    for(int i = 0; i < cur_move; i++) outfile << coorSaver[i].first << " " << coorSaver[i].second << endl;

    outfile.close();
    return true;
}

bool GoEngine::loadGame(const string& filepath){
    ifstream infile(filepath);

    if(!infile.is_open()){
        cerr << "Error: Could not open file for reading: " << filepath << endl;
        return false;
    }

    infile >> boardSize;
    initialize_board(boardSize);
    infile >> komi;
    int mv; infile >> mv;

    for(int i = 0; i < cur_move; i++){
        int x = 0, y = 0;
        infile >> x >> y;
        make_move(x, y);
    }

}