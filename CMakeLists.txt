# 1. Basic Project Setup
cmake_minimum_required(VERSION 3.20)
project(GoGame_CS160 VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Tell CMake where to find our header files
# This lets src/main.cpp write #include "Game.h"
include_directories(include)

# 3. Find SFML (vcpkg makes this work)
# We ask for version 2.6.1 and the components we need
find_package(SFML 2.6.1 REQUIRED COMPONENTS system window graphics audio)
# Note: Add 'network' if you need it.

# 4. Define our executable
# We list ALL .cpp files from the src/ folder
add_executable(GoGame_CS160
        src/main.cpp
        include/Definitions.h
        include/TextureManager.h
        src/TextureManager.cpp
        include/GoGame.h
        src/GoGame.cpp
        include/GameRenderer.h
        src/GameRenderer.cpp
        include/AudioManager.h
        src/AudioManager.cpp
        include/GoUIManager.h
        src/GoUIManager.cpp
        include/MainMenuScreen.h
        src/MainMenuScreen.cpp
        include/SettingsScreen.h
        src/SettingsScreen.cpp
        include/Board.h
        src/Board.cpp
        include/SaveLoadScreen.h
        src/SaveLoadScreen.cpp
        src/SaveScreen.cpp
        src/LoadScreen.cpp
        # Add any other .cpp files you create here
)

# 5. Link SFML to our executable
# This tells the compiler to attach the SFML libraries
target_link_libraries(GoGame_CS160 PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

# 6. [IMPORTANT] Copy assets to the build folder
# Your .exe runs from a 'cmake-build-debug' folder,
# so it can't find the 'assets' folder unless we copy it.
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(DESTINATION_DIR ${CMAKE_BINARY_DIR})

add_custom_command(TARGET GoGame_CS160 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${ASSETS_DIR}
        ${DESTINATION_DIR}/assets
        COMMENT "Copying assets..."
)