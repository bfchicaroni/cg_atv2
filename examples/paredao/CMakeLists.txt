project(paredao)
add_executable(${PROJECT_NAME} main.cpp window.cpp ball.cpp bullets.cpp
                               bar.cpp)
enable_abcg(${PROJECT_NAME})