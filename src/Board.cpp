#include "../include/board.h"
#include "KataGoRunner.h" // Your AI connector class

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
std::pair<int, int> GoEngine::spread(int x_coor, int y_coor, int dir){
    int switch_row[4] = {1, -1, 0, 0}, switch_col[4] = {0, 0, 1, -1};

    return {x_coor + switch_row[dir], y_coor + switch_col[dir]};
}

void GoEngine::delete_clump(int x_coor, int y_coor){
    std::queue< std::pair<int, int> > q;
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
    std::queue< std::pair<int, int> > q;
    q.push({x_coor, y_coor});
    int lib_count = 0; Player ini = board[x_coor][y_coor];
    std::vector<std::vector<bool>> vir_board(boardSize+1, std::vector<bool>(boardSize+1, 0));
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

bool GoEngine::reassest_board_state(int x, int y, bool& didCaptured){
    for(int i = 0; i < 4; i++){
        auto [xn, yn] = spread(x, y, i);
        if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
            if(board[xn][yn] != Player::NONE && board[xn][yn] != board[x][y]) {
                if(!check_survivability(xn, yn)) didCaptured = 1;
            }
        }
    }
    if(!check_survivability(x, y)) return false;
    cur_hash = calcZobrist();
    return checkSuperko();
}

//Zobrist Hash and SuperKo construction

void GoEngine::initZobrist(int brd) {
    std::mt19937_64 rng(123456); // fixed seed for reproducibility
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    zobristTable = std::vector<std::vector<std::vector<uint64_t>>>(brd+1, std::vector<std::vector<uint64_t>>(brd+1, std::vector<uint64_t>(2, 0)));
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
    else if(currentPlayer == Player::WHITE) currentPlayer = Player::BLACK;
}


void GoEngine::clean_up_dead(){
    for(int i = 1; i <= boardSize; i++)
        for(int j = 1; j <= boardSize; j++) if(dead[i][j] == 1) board[i][j] = Player::NONE;
}



//--------PUBLIC FUNCTION DEFINITIONS----------

void GoEngine::initialize_board(int Size){
    boardSize = Size; cur_move = 0; currentPlayer = Player::BLACK;
    board.clear(); board.assign(Size+1, std::vector<Player>(Size+1, Player::NONE));
    zobristTable.clear(); zobristTable.assign(Size+1, std::vector<std::vector<uint64_t>>(Size+1, std::vector<uint64_t>(2, 0)));
    dead = std::vector< std::vector<bool> >(boardSize+1, std::vector<bool>(boardSize+1, 0));
    superkoMap.clear();
    cur_hash = 0;
    history = {};
    coorSaver.clear();
    consecutivePass = 0;
    store_to_history();
    initZobrist(Size);
}

bool GoEngine::make_move(int x, int y, bool& didCaptured){
    cur_move++;
    std::cerr << cur_move << std::endl;
    didCaptured = 0;
    switchPlayer();
    if(board[x][y] != Player::NONE) {
        cur_move--;
        switchPlayer();
        return false;
    }
    board[x][y] = currentPlayer;
    if(!reassest_board_state(x, y, didCaptured)){
        cur_move--;
        switchPlayer();
        board = history.board[cur_move];
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
    std::cerr << cur_move << std::endl;
    switchPlayer();
    delete_branch(cur_move);
    store_to_history();
    consecutivePass++;
    coorSaver.push_back({-1, -1});
    if(consecutivePass == 2) return true;
    return false;
}

std::vector< std::vector<bool> > GoEngine::validMoves()  {
    std::vector< std::vector<bool> > valid(boardSize+1, std::vector<bool>(boardSize+1, 0));

    for (int i = 1; i <= boardSize; i++) {
        for (int j = 1; j <= boardSize; j++) {
            if (board[i][j] != Player::NONE) {
                valid[i][j] = 0;
                continue;
            }
            switchPlayer();
            board[i][j] = currentPlayer;
            bool dc = 0;
            valid[i][j] = reassest_board_state(i, j, dc);
            switchPlayer();
            board = history.board[cur_move];
        }
    }
    return valid;
}
bool GoEngine::undo_step(){
    if(cur_move == 0) return false;
    cur_move--;
    switchPlayer();
    //cout << cur_move << endl;
    superkoMap.erase(cur_hash);
    cur_hash = history.cur_hash[cur_move];
    board = history.board[cur_move];
    consecutivePass = history.consecutivePass[cur_move];
    return true;
}

bool GoEngine::redo_step(int& x, int& y, std::string& player){
    if(cur_move == history.board.size() - 1) return false;
    cur_move++;
    if (cur_move % 2 == 1) player = "B";
    else player = "W";
    switchPlayer();
    cur_hash = history.cur_hash[cur_move]; superkoMap[cur_hash]++;
    board = history.board[cur_move];
    consecutivePass = history.consecutivePass[cur_move];
    std::tie(x, y) = coorSaver[cur_move-1];
    x--;
    y--;
    std::cout << x << " " << y << std::endl;
    return true;
}

Player GoEngine::getCurrentPlayer() const{
    return currentPlayer;
}

std::vector< std::vector<Player> > GoEngine::getBoard() const{
    return board;
}

std::string toGTPCoordinate(int x, int y) {
    if (x < 1 || x > 19 || y < 1 || y > 19) return "PASS";

    std::string coord = "";

    // Convert X (1-19) to Letter (A-T, skipping I)
    // 1=A, 8=H, 9=J (skip I), ...
    char xChar = 'A' + (x - 1);
    if (x >= 9) xChar++; // Skip 'I'
    coord += xChar;

    // Convert Y (1-19) to String
    coord += std::to_string(y);

    return coord;
}

void GoEngine::deadStoneHeuristic(){
    std::cout << "--- Calculating Life & Death (Initializing Temp AI) ---" << std::endl;

    // 1. Initialize a separate, temporary bot instance
    KataGoRunner tempBot;

    // Use the relative paths that we know work
    bool started = tempBot.startEngine("./AI/katago.exe", "./AI/model.txt.gz", "./AI/cpu_config.cfg");

    if (!started) {
        std::cerr << "[Error] Could not start temp AI for heuristic." << std::endl;
        return;
    }

    // 2. Setup the "Physics" of the board
    tempBot.sendCommand("boardsize 19");
    tempBot.sendCommand("komi 6.5");
    tempBot.sendCommand("clear_board");

    // 3. Reconstruct the board state from your 2D vector
    // looping 1 to 19 (Base-1)
    for (int x = 1; x <= 19; x++) {
        for (int y = 1; y <= 19; y++) {
            // Reset dead status to 0 (alive) initially
            dead[x][y] = false;

            if (board[x][y] == Player::BLACK) {
                tempBot.sendCommand("play B " + toGTPCoordinate(x, y));
            }
            else if (board[x][y] == Player::WHITE) {
                tempBot.sendCommand("play W " + toGTPCoordinate(x, y));
            }
        }
    }

    // 4. Ask the Oracle what is dead
    std::string response = tempBot.sendCommand("final_status_list dead");

    // Response format is usually: "= A1 B2 C5\n\n"
    // We need to clean it up.

    // Remove the leading "= " if present
    size_t equalPos = response.find("=");
    if (equalPos != std::string::npos) {
        response = response.substr(equalPos + 1);
    }

    // 5. Parse the list of coordinates
    std::stringstream ss(response);
    std::string segment;

    while (std::getline(ss, segment, ' ')) {
        // Trim whitespace (newlines/spaces)
        segment.erase(0, segment.find_first_not_of(" \n\r\t"));
        segment.erase(segment.find_last_not_of(" \n\r\t") + 1);

        if (segment.length() < 2) continue; // Skip empty garbage

        // Convert GTP (e.g., "Q16") back to X,Y (Base-1)
        char colChar = std::toupper(segment[0]);
        int x = 0;
        int y = 0;

        // Parse X
        if (colChar >= 'A' && colChar <= 'H') {
            x = colChar - 'A' + 1; // A=1
        } else if (colChar >= 'J' && colChar <= 'T') {
            x = colChar - 'A'; // J(9) -> 9. 'J' is 10th letter, -1 shift for I.
        }

        // Parse Y
        try {
            y = std::stoi(segment.substr(1));
        } catch (...) { continue; }

        // 6. Update the 'dead' vector
        if (x >= 1 && x <= 19 && y >= 1 && y <= 19) {
            dead[x][y] = true;
            // Optional: Debug print
            std::cout << "Stone at " << x << "," << y << " is DEAD." << std::endl;
        }
    }
    for (int i = 1; i <= 19; i++) {
        for (int j = 1; j <= 19; j++) std::cout << dead[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << "--- Life & Death Calculation Complete ---" << std::endl;
    // tempBot is destroyed here automatically, closing the pipe.
}

std::vector< std::vector<bool> > GoEngine::getDeadState() const  {
    return dead;
}
void GoEngine::toggle_life_death(int x, int y){
    dead[x][y] = dead[x][y] ^ 1;
}

std::pair<float, float> GoEngine::calculateScore(){
    deadStoneHeuristic();
    clean_up_dead();
    std::vector< std::vector<bool> > checked(boardSize+1, std::vector<bool>(boardSize+1, 0));
    float Bpt = 0, Wpt = komi;
    for(int i = 1; i <= boardSize; i++){
        for(int j = 1; j <= boardSize; j++) if(checked[i][j] == 0){
            int cnt = 0; bool Badj = 0, Wadj = 0;
            std::queue<std::pair<int, int>> q;
            q.push({i, j});

            while(!q.empty()){
                auto [x, y] = q.front();
                //cout << x << " " << y << endl;
                q.pop();
                checked[x][y] = 1;
                cnt++;

                for(int k = 0; k < 4; k++){
                    auto [xn, yn] = spread(x, y, k);
                    //cout << xn << " " << yn << "    ";
                    if(xn > 0 && yn > 0 && xn <= boardSize && yn <= boardSize){
                        if(board[xn][yn] == Player::BLACK) Badj = 1;
                        if(board[xn][yn] == Player::WHITE) Wadj = 1;
                        if(checked[xn][yn] == 0 && board[xn][yn] == board[i][j]) {
                            checked[xn][yn] = 1;
                            q.push({xn, yn});
                        }
                    }
                }
                //cout << endl;
            }

            if(board[i][j] == Player::BLACK) Wpt += cnt;
            if(board[i][j] == Player::WHITE) Bpt += cnt;
            if(board[i][j] == Player::NONE){
                if(Badj == 0 && Wadj == 0) continue;
                if(!Wadj) Wpt += cnt;
                if(!Badj) Bpt += cnt;
            }
            std::cout << i << " " << j << ": " << Bpt << " " << Wpt << " " << Wadj << " " << Badj << std::endl;
        }
    }
    std::cout << Bpt << " " << Wpt << std::endl;
    return {Bpt, Wpt};
}

//0: PVP
//1: Easy
//2: Medium
//3: Hard

bool GoEngine::saveGame(const std::string& filepath, Difficulty mode){
    std::ofstream outfile(filepath);

    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filepath << std::endl;
        return false;
    }
    outfile << mode << std::endl;
    outfile << boardSize << std::endl;
    outfile << komi << std::endl;
    outfile << cur_move << std::endl;
    for(int i = 0; i < cur_move; i++) outfile << coorSaver[i].first << " " << coorSaver[i].second << std::endl;

    outfile.close();
    return true;
}

bool GoEngine::loadGame(const std::string& filepath, Difficulty &mode){
    std::ifstream infile(filepath);

    if(!infile.is_open()){
        std::cerr << "Error: Could not open file for reading: " << filepath << std::endl;
        return false;
    }

    int temp;
    infile >> temp;

    if (temp == 0) mode = Difficulty::NONE;
    if (temp == 1) mode = Difficulty::EASY;
    if (temp == 2) mode = Difficulty::MEDIUM;
    if (temp == 3) mode = Difficulty::HARD;

    infile >> boardSize;
    initialize_board(boardSize);
    infile >> komi;
    int mv; infile >> mv;
    for(int i = 0; i < mv; i++){
        int x = 0, y = 0;
        infile >> x >> y;
        bool dc = 0;
        if (x == -1) pass_move();
        else make_move(x, y, dc);
    }
    infile.close();
    return true;
}