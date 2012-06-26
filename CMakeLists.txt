cmake_minimum_required(VERSION 2.8)

set (CMAKE_C_COMPILER arm-none-linux-gnueabi-gcc)
set (CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)

add_subdirectory (thirdparty/objLoader)

include_directories (thirdparty)
include_directories (/scratch/tiahan01/sdk_odroida/include)
link_directories (/scratch/tiahan01/source_depot/PR400/trunk_profiling/lib)

add_executable (fuxi
    src/main.cpp
    src/matrix.cpp
    src/shader.cpp
)

target_link_libraries (fuxi
    objLoader EGL GLESv2
)
