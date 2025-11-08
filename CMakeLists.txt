cmake_minimum_required(VERSION 3.10)
project(ras_muafiyet C)
set(CMAKE_C_STANDARD 99)

# saw this on the internet probably not need but still
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) 

add_executable(${PROJECT_NAME} "src/main.c")

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/lib/raylib.lib" winmm.lib)

target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/")
