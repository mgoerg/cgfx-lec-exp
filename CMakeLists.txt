cmake_minimum_required(VERSION 3.13)
project(ex06)

set(CMAKE_CONFIGURE_DEPENDS data)

file(COPY data DESTINATION .)
file(COPY glew32.dll SDL2.dll DESTINATION .)



#ls src/*.h -1
add_executable(xyz
    src/lightsource.cpp
    src/main.cpp
    src/mesh.cpp
    src/shader.cpp
    src/tilemap3d.cpp
    src/camera.cpp
    src/importMagicaVoxel.cpp
    src/utils.cpp
    src/renderer.cpp

    src/lightsource.h
    src/mesh.h
    src/shader.h
    src/tilemap3d.h
    src/camera.h
    src/importMagicaVoxel.h
    src/utils.h
    src/renderer.h
)

target_link_libraries(xyz PRIVATE
    mingw32 
    glew32 
    SDL2main 
    SDL2 
    opengl32 
)

target_include_directories(xyz PRIVATE
    /c/mingw_dev_lib/sdl/include
    /c/mingw_dev_lib/glew/include 
	/usr/include 
	src 
)

# -Wl,-subsystem,windows gets rid of the console window 
# -m32 force 32bit

target_compile_options(xyz PRIVATE
    -m32
    -g
    -Wall
    -Wextra
    -O2
    #-Wl,-subsystem,windows 
)

target_link_directories(xyz PRIVATE
    /c/mingw_dev_lib/sdl/lib
	/c/mingw_dev_lib/glew/lib/Release/Win32
	/c/mingw_dev_lib/glew/bin/Release/Win32
)
