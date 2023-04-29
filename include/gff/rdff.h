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

typedef struct gff_rdff_s {
    gff_rdff_header_t header;
    char data[2048];
} gff_rdff_t;

enum {
    RDFF_OBJECT    = 1,
    RDFF_CONTAINER = 2,
    RDFF_DATA      = 3,
    RDFF_NEXT      = 4,
    RDFF_END       =-1
};

extern int gff_rdff_load(gff_file_t *f, int res_id, gff_rdff_t *rdff);
extern int gff_rdff_to_object(gff_rdff_t *rdff, gff_object_t *ssi);

#endif
