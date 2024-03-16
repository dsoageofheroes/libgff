set(LIBGFF_SRCS
    debug.c
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
    gpl/gpl.c
    gpl/manager.c
    gpl/parse.c
    gpl/string.c
    gpl/state.c
    #gpl/var.c
)

include_directories(../include)
include_directories(../ext)
add_library(gff SHARED ${LIBGFF_SRCS})

install(TARGETS gff DESTINATION .)
