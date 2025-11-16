cmake_minimum_required(VERSION 3.16)
project(hnanh_gogame_local)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)

# FIX: Fetch SFML 2.6.1 (Use the correct tag)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

add_executable(hnanh_gogame_local UI.cpp)

# Link SFML headers (no longer need the SYSTEM workaround as 2.x is standard)
target_include_directories(hnanh_gogame_local
        PRIVATE
        ${SFML_SOURCE_DIR}/include
)

# Link SFML to your executable
target_link_libraries(hnanh_gogame_local
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
)