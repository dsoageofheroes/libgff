add_executable(gfftool gfftool.c gfftool-write.c gfftool-print.c)
target_link_libraries(gfftool gff xmi2mid)
include_directories(../include)
