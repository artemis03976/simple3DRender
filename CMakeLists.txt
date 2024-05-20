cmake_minimum_required(VERSION 3.27)
project(3DRenderer C)

set(CMAKE_C_STANDARD 99)
set(SDL_DIR "C:/Users/nichenjie/SDL2-2.30.3/x86_64-w64-mingw32")

# 设置要包含的头文件的路径
include_directories(${SDL_DIR}/include)
# 设置要关联的库的路径
link_directories(${SDL_DIR}/lib)

add_executable(3DRenderer src/main.c
        src/display.c
        src/display.h
        src/vector.c
        src/vector.h
        src/mesh.c
        src/mesh.h
        src/triangle.c
        src/triangle.h
        src/array.c
        src/array.h
        src/matrix.c
        src/matrix.h
        src/light.c
        src/light.h
        src/texture.c
        src/texture.h
        src/swap.c
        src/swap.h
        src/upng.c
        src/upng.h
        src/camera.c
        src/camera.h
        src/clipping.c
        src/clipping.h)

target_link_libraries(3DRenderer
        mingw32
        SDL2main
        SDL2
)

