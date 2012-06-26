cmake_minimum_required(VERSION 2.8)

set (CMAKE_C_COMPILER arm-none-linux-gnueabi-gcc)
set (CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)

include_directories (/scratch/tiahan01/sdk_odroida/include)
link_directories (/scratch/tiahan01/source_depot/PR400/trunk_profiling/lib)

add_executable (cube_example
    src/cube.cpp
    src/matrix.cpp
    src/shader.cpp
)

target_link_libraries (cube_example
    EGL GLESv2
)
