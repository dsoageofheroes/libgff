#include <stdlib.h>

#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/object.h"

extern int gff_ojff_read(gff_file_t *f, int object_index, gff_ojff_t *ojff) {
    if (!ojff) { return EXIT_FAILURE; }

    if (object_index < 0) { object_index *= -1; }

    gff_chunk_header_t chunk;
    if (gff_find_chunk_header(f, &chunk, GFF_OJFF, object_index)) {
        goto head_error;
    }

    if (gff_read_chunk(f, &chunk, ojff, sizeof(gff_ojff_t)) != sizeof(gff_ojff_t)) {
        goto read_error;
    }

    return EXIT_SUCCESS;
read_error:
head_error:
    return EXIT_FAILURE;
}
