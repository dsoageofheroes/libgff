#include <stdlib.h>

#include "gff.h"
#include "gpl.h"
#include "scmd.h"

//static unsigned char scmds[SCMD_MAX][SCMD_MAX];
static gff_scmd_t empty_scmd = {
    .bmp_idx = 0,
    .delay = 0,
    .flags = SCMD_LAST,
    .xoffset = 0,
    .yoffset = 0,
    .xoffsethot = 0,
    .yoffsethot = 0,
    .soundidx = 0
};

/*
static void print_scmd(scmd_t *scmd) {
    printf("bmp_idx = %d, delay = %d, flags = 0x%x, xoffset = %d, yoffset = %d, "
        "xoffsethot = %d, yoffsethot = %d, soundidx = %d\n",
        scmd->bmp_idx,
        scmd->delay,
        scmd->flags,
        scmd->xoffset,
        scmd->yoffset,
        scmd->xoffsethot,
        scmd->yoffsethot,
        scmd->soundidx
    );
}
*/

extern gff_scmd_t* gff_scmd_get(gff_scmd_t *scmd_entry, const int index) {
    if (scmd_entry == NULL) { return NULL; }
    if (index < 0 || index >= SCMD_MAX_SIZE) {
        error("index for get_script is out of bounds!(%d)\n", index);
        return NULL;
    }
    gff_scmd_t *scmds = (gff_scmd_t*)(scmd_entry + (SCMD_MAX_SIZE * 2));
    uint16_t scmd_idx = *((uint16_t*)scmd_entry + index);
    return scmds + scmd_idx;
}

extern gff_scmd_t* gff_scmd_empty() {
    return &empty_scmd;
}

extern int gff_scmd_read(gff_file_t *f, const int res_id, gff_scmd_t **scmd) {
    if (res_id <= 0 || res_id >= SCMD_MAX) { return EXIT_FAILURE; } // needs a better check...

    gff_chunk_header_t chunk;
    if (gff_find_chunk_header(f, &chunk, GFF_SCMD, res_id)) {
        goto header_error;
    }

    if (chunk.length > SCMD_MAX) {
        error("chunk length (%d) is larger than SCMD_MAX (%d)\n", chunk.length, SCMD_MAX);
        goto len_error;
    }

    *scmd = malloc(chunk.length);
    if (*scmd == NULL) {
        goto memory_error;
    }

    if (gff_read_chunk(f, &chunk, *scmd, chunk.length) < chunk.length) {
        goto read_error;
    }

    return EXIT_SUCCESS;
read_error:
    free(*scmd);
    *scmd = NULL;
memory_error:
len_error:
header_error:
    return EXIT_FAILURE;
}

extern int gff_scmd_next_pos(const gff_scmd_t *scmd, const int scmd_index) {
    if (!scmd) { return -1; }
    if (scmd[scmd_index].flags & SCMD_LAST) {
        return scmd_index;
    }
    if (scmd[scmd_index].flags & SCMD_JUMP) {
        return 0;
    }
    return scmd_index + 1;
}

extern int gff_scmd_total_delay(const gff_scmd_t *scmd, int scmd_index) {
    int sum = 0;
    if (!scmd) { return 0; }

    while (!(scmd[scmd_index].flags & SCMD_LAST) && !(scmd[scmd_index].flags & SCMD_JUMP)) {
        sum += scmd[scmd_index].delay;
        scmd_index++;
    }

    sum += scmd[scmd_index].delay;

    return (scmd[scmd_index].flags & SCMD_JUMP) ? -1 : sum;
}
