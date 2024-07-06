#ifndef GFF_RDFF_H
#define GFF_RDFF_H

#include <stdint.h>

#include "common.h"
#include "object.h"

typedef struct gff_rdff_header_s {
    int8_t  load_action;
    int8_t  blocknum;
    int16_t type;
    int16_t index;
    int16_t from;
    int16_t len;
} gff_rdff_header_t;

#define RDFF_BUF_SIZE (2048)

typedef struct gff_rdff_s {
    gff_rdff_header_t header;
    char data[RDFF_BUF_SIZE];
} gff_rdff_t;

enum {
    RDFF_OBJECT    = 1, // Usually an item.
    RDFF_CONTAINER = 2,
    RDFF_DATA      = 3,
    RDFF_NEXT      = 4,
    RDFF_END       =-1
};

enum {
    RDFF_DATA_ITEM = 1,
    RDFF_DATA_COMBAT = 2,
    RDFF_DATA_CHARREC = 3,
    RDFF_DATA_ITEM1R = 4,
    RDFF_DATA_MINI = 5,
    RDFF_DATA_NAMEIX = 6,
};

extern int gff_rdff_load(gff_file_t *f, int res_id, gff_rdff_t *rdff);
extern int gff_rdff_to_object(gff_rdff_t *rdff, gff_object_t *obj);

#endif
