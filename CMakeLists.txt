# 1. Basic Project Setup
cmake_minimum_required(VERSION 3.20)
project(GoGame_CS160 VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Tell CMake where to find our header files
include_directories(include)

# ==============================================================================
# 3. THE "PEACE TREATY" BLOCK (UNIVERSAL SFML SETUP)
# This block automatically detects which computer is running the code.
# ==============================================================================

# Check if the local "SFML-2.6.1" folder exists (This matches YOUR setup)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.1/CMakeLists.txt")
    message(STATUS ">> Detected Local SFML folder. Mode: STATIC BUILD (For You)")

    # Force Static build to fix the "windres" error on your machine
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    # Build SFML from the local folder
    add_subdirectory(SFML-2.6.1)

    # If the folder is missing, assume we are on HER computer (MSYS2)
else()
    message(STATUS ">> Local SFML not found. Mode: SYSTEM PACKAGE (For Her)")

    # Use the Package Manager (how she was doing it originally)
    find_package(SFML 2.6.1 REQUIRED COMPONENTS system window graphics audio)
endif()

# ==============================================================================

# 4. Define our executable
# I kept her new files (SaveScreen/LoadScreen) in this list
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
        include/GameOverScreen.h
        src/GameOverScreen.cpp
        src/ToggleScreen.cpp
        include/ToggleScreen.h
        src/KataGoRunner.cpp
        include/KataGoRunner.h
)

# 5. Link SFML to our executable
# This works for both methods now
target_link_libraries(GoGame_CS160 PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

# 6. Copy assets to the build folder
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(DESTINATION_DIR ${CMAKE_BINARY_DIR})

add_custom_command(TARGET GoGame_CS160 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${ASSETS_DIR}
        ${DESTINATION_DIR}/assets
        COMMENT "Copying assets..."
)

# 7. [AUTO-FIX] Copy OpenAL32.dll (Audio Fix for YOU)
# This puts the sound driver next to the .exe so you don't crash
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/SFML-2.6.1/extlibs/bin/x64/openal32.dll")
    add_custom_command(TARGET GoGame_CS160 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/SFML-2.6.1/extlibs/bin/x64/openal32.dll"
            $<TARGET_FILE_DIR:GoGame_CS160>
            COMMENT "Auto-fixing Audio DLL..."
    )
endif()