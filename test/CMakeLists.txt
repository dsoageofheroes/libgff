#include (FindLua)
#find_package(ZLIB REQUIRED )

include_directories(../include)
include_directories(../unity)

set(TEST_NAMES 
    ds1-basic
    ds1-image
    ds1-map
    ds1-resource
    ds1-segobjex
)

foreach(TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME}_app ${TEST_NAME}.c)
    target_link_libraries(${TEST_NAME}_app unity gff xmi2mid)
    add_test(
        NAME              ${TEST_NAME} 
        COMMAND           ${TEST_NAME}_app
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../
        )
endforeach()
