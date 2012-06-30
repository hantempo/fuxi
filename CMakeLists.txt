cmake_minimum_required(VERSION 2.8)

if (ARM)
    set (CMAKE_C_COMPILER arm-none-linux-gnueabi-gcc)
    set (CMAKE_CXX_COMPILER arm-none-linux-gnueabi-g++)
endif ()

add_subdirectory (thirdparty/objLoader)

include_directories (thirdparty)

add_library (common STATIC
    src/common.cpp
    src/math3d.cpp
)

if (ARM)
    include_directories (/scratch/tiahan01/sdk_odroida/include)
    link_directories (/scratch/tiahan01/source_depot/PR400/trunk_stable/lib)

    add_executable (fuxi
        src/main.cpp
        src/context.cpp
        src/geometry.cpp
        src/shader.cpp
    )

    target_link_libraries (fuxi
        common objLoader EGL GLESv2
    )
endif ()

add_executable (generate_normal
    src/generate_normal.cpp
)

target_link_libraries (generate_normal
    common objLoader
)
