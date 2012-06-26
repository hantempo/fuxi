include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library (objLoader STATIC
    list.cpp
    objLoader.cpp
    obj_parser.cpp
    string_extra.cpp
)
