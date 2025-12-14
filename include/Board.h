#ifndef GO_ENGINE_H
#define GO_ENGINE_H

#include <bits/stdc++.h>


enum class Player { NONE, WHITE, BLACK };

class GoEngine {
private: 
    std::vector<std::vector<Player>> board;
    std::vector< std::vector<bool> > dead;
    int boardSize, consecutivePass; float komi = 6.5;
    Player currentPlayer;
    int cur_move; uint64_t cur_hash;
    std::vector<std::pair<int, int>> coorSaver;
    std::vector<std::vector<std::vector<uint64_t>>> zobristTable;
    std::unordered_map<uint64_t, int> superkoMap;
    struct hist{
        std::vector<std::vector<std::vector<Player>>> board;
        std::vector<uint64_t> cur_hash;
        std::vector<int> consecutivePass;
    } history;

    void switchPlayer();
    int convert_numeral(Player a);

    void store_to_history();
    void delete_branch(int move);

    std::pair<int, int> spread(int x_coor, int y_coor, int dir);
    void delete_clump(int x_coor, int y_coor);
    bool check_survivability(int x_coor, int y_coor);

    bool reassest_board_state(int x, int y, bool& didCaptured);


    void initZobrist(int brd);
    uint64_t calcZobrist();
    bool checkSuperko();


    void clean_up_dead();
public:
    
    // Constructor
    void initialize_board(int boardSize);

    // Returns true if the move was valid and successful
    bool make_move(int x, int y, bool& didCaptured);
    bool pass_move();
    std::vector<std::vector<bool>> validMoves();
    //Undo/Redo by one move
    //Doesn't handle overflow/underflow states
    bool undo_step();
    bool redo_step(int& x, int& y, std::string& player);

    // Returns the current player
    Player getCurrentPlayer() const;

    // Returns the state of the board
    std::vector< std::vector<Player> > getBoard() const;

    // --- SCORING ---
    // Returns a pair: {BlackScore, WhiteScore}
    void deadStoneHeuristic();
    std::vector< std::vector<bool> > getDeadState() const;
    void toggle_life_death(int x, int y);
    std::pair<float, float> calculateScore();

    // --- SAVE/LOAD ---
    bool saveGame(const std::string& filepath, const std::string& mode);
    bool loadGame(const std::string& filepath, std::string& mode);
};

#endif