include(ExternalProject)
cmake_minimum_required(VERSION 3.18)

project(libgff VERSION 0.6 LANGUAGES C)

add_definitions(-DDEBUG)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    #add_compile_options(-fsanitize=address)
    #add_link_options(-fsanitize=address)
endif()

#set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -fsanitize=address -g -fno-omit-frame-pointer")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -g -fno-omit-frame-pointer")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -Wall -Werror -fsanitize=address -g -fno-omit-frame-pointer")

add_subdirectory(ext/)
add_subdirectory(src/)
add_subdirectory(tools/)

enable_testing()
add_subdirectory(unity/)
add_subdirectory(test/)
