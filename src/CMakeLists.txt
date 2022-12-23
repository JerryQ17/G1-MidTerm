cmake_minimum_required(VERSION 3.23)
project(G1-MidTerm)
set(CMAKE_C_STANDARD 23)

add_executable(main main.c game.c dice.c chess.c draw.c variables.c FlightChess.h)

target_link_libraries(main mingw32)
target_link_libraries(main SDL2main)
target_link_libraries(main SDL2)
target_link_libraries(main SDL2_image)
target_link_libraries(main SDL2_ttf)
target_link_libraries(main SDL2_mixer)