add_executable(gffmod gffmod.c gffmod-write.c gffmod-print.c)
target_link_libraries(gffmod gff xmi2mid)
include_directories(../include)
