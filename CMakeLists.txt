cmake_minimum_required(VERSION 2.8)

set (CMAKE_BUILD_TYPE Debug)

if (ARM)
    set (CMAKE_C_COMPILER arm-none-linux-gnueabi-gcc)
    set (CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)
endif ()

add_subdirectory (thirdparty/objLoader)

include_directories (thirdparty)
include_directories (include)

add_library (common STATIC
    src/common.cpp
    src/math3d.cpp
    src/image.cpp
)

set (gles_common
    src/main.cpp
    src/context.cpp
    src/geometry.cpp
    src/shader.cpp
    src/program.cpp
)

if (ARM)
    link_directories (/scratch/tiahan01/source_depot/PR400/trunk_stable/lib)

    add_executable (fuxi
        src/context_fbdev.cpp
        ${gles_common}
    )
else ()
    link_directories (~/lib)

    add_executable (fuxi
        src/context_x11.cpp
        ${gles_common}
    )
endif ()

target_link_libraries (fuxi
    common objLoader EGL GLESv2
)

