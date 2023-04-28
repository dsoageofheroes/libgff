#include <stdlib.h>
#include <string.h>

#include "gff.h"
#include "gfftypes.h"
#include "item.h"
#include "rdff.h"
#include <string.h>


#define BUF_MAX (1<<12)

extern int gff_item_read(gff_file_t *f, int id, ds1_item_t *item) {
    gff_rdff_t entry;

    if (item == NULL) { goto null_error; }
    if (id < 0) { id *= -1; }

    if (gff_rdff_load(f, id, &entry)) {
        goto rdff_load_error;
    }

    /*
    gff_read_chunk(OBJEX_GFF_INDEX, &chunk, buf, chunk.length);
    */

    //if(entry->type != RDFF_OBJECT && entry->type != RDFF_CONTAINER) { return 0; }
    //if(entry.header.type != RDFF_OBJECT && entry.header.type != RDFF_CONTAINER) {
    if(entry.header.type != RDFF_OBJECT) {
        goto not_rdff; // Containers are not items.
    }

    memcpy(item, (entry.data), sizeof(ds1_item_t));

    return EXIT_SUCCESS;
not_rdff:
rdff_load_error:
null_error:
    return EXIT_FAILURE;
}
