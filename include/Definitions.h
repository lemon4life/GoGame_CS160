#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// --- Constants ---
const int STONESNUM = 3;
const int BOARD_SIZE = 19;
const int WINDOW_WIDTH = 1050;
const int WINDOW_HEIGHT = 850;
const int GRID_OFFSET = 50;
const float STONE_RADIUS = 20.0f;
const int PANEL_WIDTH = 200;
const float BOARD_MAX_WIDTH = WINDOW_WIDTH - PANEL_WIDTH;
const float CELL_SIZE = (WINDOW_HEIGHT - 2 * GRID_OFFSET) / (BOARD_SIZE - 1.0f);

// UI Constants
const int BUTTON_HEIGHT = 40;
const int BUTTON_PADDING = 10;
const int BUTTON_START_X_GAME_UI = WINDOW_WIDTH - PANEL_WIDTH + BUTTON_PADDING;
const int BUTTON_START_Y_GAME_UI = GRID_OFFSET;

// Enums
enum class Stone { Empty, White, Black };
enum class GameState { MENU, PLAYING, SETTINGS, EXIT, SAVE_MENU, LOAD_MENU,  GAME_OVER, SCORING};

// Structs
struct Move {
    int x;
    int y;
    Stone player;
};

struct GameSettings {
    int boardThemeIndex = 0; 
    int stoneStyleIndex = 0; 
    int bgmIndex = 0; // <--- NEW: Track selected music
    bool soundEnabled = true;
    bool musicEnabled = true;
    float volume = 50.0f;
    // float bgmvolume = 50.0f;
    // float effectvolume = 50.0f;

    std::string getBoardImagePath() const {
        if (boardThemeIndex == 1) return "assets/img/background/ocean.jpg";
        if (boardThemeIndex == 2) return "assets/img/background/galaxy.jpg";
        return "assets/img/background/wood.jpg"; 
    }
};