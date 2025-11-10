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
            reassest_board_state(board);
        }

    private:
        int boardsize, move;

        vector<vector<int>> board, root, no_dsu, dsu_size;
        vector<int> lib_num;
        vector<vector<vector<int>>> history;
        
        void reset_board(int sz){
            board.clear();
            board.assign(sz+1, vector<int>(sz+1, 0));
            history.clear();
            history.push_back(board);
        }

        int findroot(vector<vector<int>>& rt, int x, int y){
            if(rt[x][y] = x*19+y) return x*19+y;
            return rt[x][y] = findroot(rt, rt[x][y]/19, rt[x][y]%19);
        }

        void merge_sets(int x1, int y1, int x2, int y2){
            
        }
        void reassest_board_state(vector<vector<int>>& brd){
            
        }
        bool ko_check(){
            if(move < 2) return true;

            if(board == history[move-2]) return false;
            return true;
        }
};