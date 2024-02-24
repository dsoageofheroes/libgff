set(LIBGFF_SRCS
    image.c
    item.c
    char.c
    gff.c
    gfftypes.c
    manager.c
    name.c
    rdff.c
    region.c
    object.c
    scmd.c
    gui.c
    xmi.c
)

include_directories(../include)
include_directories(../ext)
add_library(gff SHARED ${LIBGFF_SRCS})

install(TARGETS gff DESTINATION .)
