#include <bits/stdc++.h>
using namespace std;

class Board {
    public:
        vector< vector<int> > board;
        int boardsize, consecutive_pass;

        void initialize_board(int sz){
            boardsize = sz; cur_move = 0;
            reset_board(boardsize);
            consecutive_pass = 0;
            initZobrist(sz);
        }

        void undo_step(){
            cur_move--;
            cout << cur_move << endl;
            superkoMap.erase(cur_hash);
            cur_hash = history.cur_hash[cur_move];
            board = history.board[cur_move];
        }

        void redo_step(){
            cur_move++;
            cur_hash = history.cur_hash[cur_move]; superkoMap[cur_hash]++;
            board = history.board[cur_move];
        }

        bool make_move(int x, int y){
            cur_move++;
            
            if(board[x][y] != 0) {
                cur_move--;
                return false;
            }
            board[x][y] = cur_move % 2 + 1;
            if(!reassest_board_state(x, y)){
                undo_step();
                //cout << "Move: " << cur_move << endl;
                //cout << "Again!" << endl;
                return false;
            }
            delete_branch(cur_move);
            store_to_history();
            consecutive_pass = 0;
            return true;
        }

        void pass_move(){
            cur_move++;
            delete_branch(cur_move);
            store_to_history();
            consecutive_pass++;
            if(consecutive_pass == 2);
        }

    private:
        int cur_move; uint64_t cur_hash;

        vector<vector<vector<uint64_t>>> zobristTable;
        unordered_map<uint64_t, int> superkoMap;
        struct hist{
            vector<vector<vector<int>>> board;
            vector<uint64_t> cur_hash;
        } history;

        int switch_row[4] = {-1, 1, 0, 0}, switch_col[4] = {0, 0, -1, 1};

        void store_to_history(){
            history.board.push_back(board);
            history.cur_hash.push_back(cur_hash);
            superkoMap[cur_hash]++;
        }

        void delete_branch(int mv){
            while(history.board.size() > mv){
                history.board.pop_back();
                history.cur_hash.pop_back();
            }
        }

        void reset_board(int sz){
            board.clear(); board.assign(sz+1, vector<int>(sz+1, 0));
            zobristTable.clear(); zobristTable.assign(sz+1, vector<vector<uint64_t>>(sz+1, vector<uint64_t>(2, 0)));

            superkoMap.clear();
            cur_hash = 0;
            history = {};
            store_to_history();
            //history.push_back(board);
        }

        pair<int, int> num_to_coor(int a){
            return {a / (boardsize+1), a % (boardsize+1)};
        }

        int coor_to_num(int x, int y){
            return x * (boardsize+1) + y;
        }

        // Verify Move And Process Board

        void delete_clump(int x_coor, int y_coor){
            queue< pair<int, int> > q;
            int ini = board[x_coor][y_coor];
            q.push({x_coor, y_coor});

            while(!q.empty()){
                auto [x, y] = q.front();
                //cout << x << " " << y << endl;
                q.pop();
                board[x][y] = 0;
                for(int i = 0; i < 4; i++){
                    int xn = x + switch_row[i], yn = y + switch_col[i];
                    if(xn > 0 && yn > 0 && xn <= boardsize && yn <= boardsize){
                        if(board[xn][yn] == ini) q.push({xn, yn});
                    }
                }
            }
        }

        bool check_survivability(int x_coor, int y_coor){
            queue< pair<int, int> > q;
            q.push({x_coor, y_coor});
            int lib_count = 0, ini = board[x_coor][y_coor];
            vector<vector<bool>> vir_board(boardsize+1, vector<bool>(boardsize+1, 0));
            while(!q.empty()){
                auto [x, y] = q.front();
                //cout << x << " " << y << endl;
                vir_board[x][y] = 1;
                q.pop();

                for(int i = 0; i < 4; i++){
                    int xn = x + switch_row[i], yn = y + switch_col[i];
                    if(xn > 0 && yn > 0 && xn <= boardsize && yn <= boardsize){
                        if(board[xn][yn] == 0) lib_count++;
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

        bool reassest_board_state(int x, int y){
            
            for(int i = 0; i < 4; i++){
                int xn = x + switch_row[i], yn = y + switch_col[i];
                if(xn > 0 && yn > 0 && xn <= boardsize && yn <= boardsize){
                    if(board[xn][yn] != 0 && board[xn][yn] != board[x][y]) {
                        check_survivability(xn, yn);
                    }
                }
            }
            if(!check_survivability(x, y)) return false;
            cur_hash = calcZobrist();
            return checkSuperko();
        }



        //Zobrist Hash and SuperKo construction

        void initZobrist(int brd) {
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

        uint64_t calcZobrist(){
            uint64_t val = 0;
            for(int i = 1; i <= boardsize; i++){
                for(int j = 1; j <= boardsize; j++) if(board[i][j] != 0) val ^= zobristTable[i][j][board[i][j]-1];
            }

            return val;
        }

        bool checkSuperko(){
            if(superkoMap.count(cur_hash)) return false;
            return true;
        }
} brd;

int main(){
    int n;
    cin >> n;
    brd.initialize_board(7);
    cout << "***" << endl;
    while(n--){
        int t;
        cin >> t;
        if(t == 1) brd.undo_step();
        else if(t == 2) brd.redo_step();
        else{
            //cout << "*" << endl;
            int x, y;
            while(true){
                cin >> x >> y;
                bool tmp = brd.make_move(x, y);
                if(tmp) break;
            }
        }

        for(int i = 1; i <= brd.boardsize; i++){
            for(int j = 1; j <= brd.boardsize; j++) cout << brd.board[i][j] << " ";
            cout << endl;
        }
    }
    return 0;
}
