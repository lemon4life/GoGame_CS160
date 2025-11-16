#include <bits/stdc++.h>
using namespace std;

class Board {
    public:
        void initilize_board(int sz){
            boardsize = sz; move = 0;
            reset_board(boardsize);
        }

        bool make_move(int x, int y){
            move++;
            if(board[x][y] != 0) return false;
            board[x][y] = move % 2 + 1;
            reassest_board_state(x, y);
        }

    private:
        int boardsize, move;

        vector<vector<int>> board, root, no_dsu, set_size;
        vector<int> lib_num;
        struct hist{
            vector<vector<vector<int>>> board, root, no_dsu, set_size;
            vector<vector<int>> lib_num;
        } history;

        int switch_row[4] = {-1, 1, 0, 0}, switch_col[4] = {0, 0, -1, 1};

        void reset_board(int sz){
            board.clear();
            board.assign(sz+1, vector<int>(sz+1, 0));
            //history.clear();
            //history.push_back(board);
        }

        pair<int, int> num_to_coor(int a){
            return {a / boardsize, a % boardsize};
        }

        int coor_to_num(int x, int y){
            return x * boardsize + y;
        }

        int findroot(int x, int y){
            if(root[x][y] == coor_to_num(x, y)) return root[x][y];
            auto [x_par, y_par] = num_to_coor(root[x][y]);
            return root[x][y] = findroot(x_par, y_par);
        }

        void merge_sets(int x1, int y1, int x2, int y2){
            tie(x1, y1) = num_to_coor(findroot(x1, y1));
            tie(x2, y2) = num_to_coor(findroot(x2, y2));

            if(x1 == x2 && y1 == y2) return;

            if(set_size[x1][y1] < set_size[x2][y2]){
                swap(x1, x2);
                swap(y1, y2);
            }

            root[x2][y2] = coor_to_num(x1, y1);
            set_size[x1][y1] += set_size[x2][y2];
            lib_num[no_dsu[x1][y1]] += lib_num[no_dsu[x2][y2]] - 1; 
        }

        void reassest_board_state(int x, int y){
            for(int i = 0; i < 4; i++){
                int xn = x + switch_row[i], yn = y + switch_col[i];
                if(xn > 0 && yn > 0 && xn <= boardsize && yn <= boardsize && board[xn][yn] == 0);
            }
        }
        bool ko_check(){
            if(move < 2) return true;

            //if(board == history[move-2]) return false;
            return true;
        }
};