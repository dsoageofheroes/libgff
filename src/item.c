#include <stdlib.h>
#include <string.h>

#include "gff/debug.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/item.h"
#include "gff/manager.h"
#include "gff/rdff.h"
#include <string.h>

#define BUF_MAX (1<<12)

static ds_item1r_t *item1rs = NULL;
static char *names          = NULL;
static uint32_t     num_item1rs = 0, num_names = 0;

extern int gff_item_read(gff_file_t *f, int id, ds1_item_t *item) {
    gff_rdff_t entry;

    if (item == NULL) { goto null_error; }
    if (id < 0) { id *= -1; }

    if (gff_read_rdff(f, id, &entry)) {
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

extern int gff_item_load(gff_manager_t *man, ds1_item_t *item, int32_t id) {
    gff_item_read(man->ds1.segobjex, id, item);
    return EXIT_SUCCESS;
}

extern int gff_read_it1r(gff_file_t *gff, int res_id, ds_item1r_t **item1rs, uint32_t *num_item1rs) {
    gff_chunk_header_t chunk;

    gff_find_chunk_header(gff, &chunk, GFF_IT1R, 1);
    if (chunk.length == 0 || (chunk.length % sizeof(ds_item1r_t)) != 0) {
        printf("Unable to load header from IT1R.\n");
        goto header_error;
    }

    *item1rs = malloc(chunk.length);
    *num_item1rs = chunk.length / sizeof(ds_item1r_t);
    //printf("chunk.length = %d, num_item1rs = %ld\n", chunk.length, num_item1rs);
    //exit(1);
    size_t amt = gff_read_chunk(gff, &chunk, *item1rs, chunk.length);
    if (amt != chunk.length) {
        printf("Unable to read IT1R.\n");
        goto read_error;
    }
    //printf("%d: %d\n", 4, item1rs[4].base_AC);
    //exit(1);

    gff_find_chunk_header(gff, &chunk, GFF_NAME, 1);
    names = malloc(chunk.length);
    num_names = chunk.length / 25;
    amt = gff_read_chunk(gff, &chunk, names, chunk.length);
    if (amt != chunk.length) {
        printf("Can't read name\n");
        goto name_read_error;
    }

    return EXIT_SUCCESS;
name_read_error:
read_error:
    free(*item1rs);
    *item1rs = NULL;
header_error:
    return EXIT_FAILURE;
}

extern int gff_manager_ds1_read_item1r(gff_manager_t *man, const int32_t item_idx, ds_item1r_t *item1r) {
    gff_chunk_header_t chunk;
    char *buf = NULL;

    gff_find_chunk_header(man->ds1.gpl, &chunk, GFF_IT1R, 1);
    if (chunk.length == 0 || (chunk.length % sizeof(ds_item1r_t)) != 0) {
        printf("Unable to load header from IT1R.\n");
        goto header_error;
    }

    uint32_t num_item1rs = chunk.length / sizeof(ds_item1r_t);
    buf = malloc(chunk.length);
    size_t amt = gff_read_chunk(man->ds1.gpl, &chunk, buf, chunk.length);

    if (amt != chunk.length) {
        printf("Unable to read IT1Rs.\n");
        goto read_error;
    }

    if (item_idx >= num_item1rs) {
        printf("item_idx out of range.\n");
        goto index_error;
    }

    memcpy(item1r, buf + sizeof(ds_item1r_t) * item_idx, sizeof(ds_item1r_t));
    free(buf);

    return EXIT_SUCCESS;
index_error:
read_error:
header_error:
    if (buf) { free(buf); }
    return EXIT_FAILURE;
}

extern int gff_manager_ds1_read_name(gff_manager_t *man, const int32_t name_idx, char *buf) {
    gff_chunk_header_t chunk;
    char *names = NULL;

    if (gff_find_chunk_header(man->ds1.gpl, &chunk, GFF_NAME, 1)) {
        debug("Unable to read NAME.\n");
        goto chunk_error;
    }

    names = malloc(chunk.length);
    uint32_t num_names = chunk.length / 25;
    uint32_t amt = gff_read_chunk(man->ds1.gpl, &chunk, names, chunk.length);

    if (amt != chunk.length) {
        printf("Can't read names.\n");
        goto name_read_error;
    }

    if (name_idx >= num_names || name_idx < 0) {
        debug("name_idx out of range.\n");
        goto range_error;
    }

    memcpy(buf, names + 25 * name_idx, 25);

    free(names);

    return EXIT_SUCCESS;

range_error:
name_read_error:
chunk_error:
    if (names) { free(names); }
    return EXIT_FAILURE;
}

static int get_item1rs(gff_manager_t *man) {
    return gff_read_it1r(man->ds1.gpl, 1, &item1rs, &num_item1rs);
    /*
    gff_chunk_header_t chunk;

    if (item1rs) { return EXIT_SUCCESS; }

    gff_find_chunk_header(man->ds1.gpl, &chunk, GFF_IT1R, 1);
    if (chunk.length == 0 || (chunk.length % sizeof(ds_item1r_t)) != 0) {
        printf("Unable to load header from IT1R.\n");
        return EXIT_FAILURE;
    }

    item1rs = malloc(chunk.length);
    num_item1rs = chunk.length / sizeof(ds_item1r_t);
    //printf("chunk.length = %d, num_item1rs = %ld\n", chunk.length, num_item1rs);
    //exit(1);
    size_t amt = gff_read_chunk(man->ds1.gpl, &chunk, item1rs, chunk.length);
    if (amt != chunk.length) {
        printf("Unable to read IT1R.\n");
        return EXIT_FAILURE;
    }
    //printf("%d: %d\n", 4, item1rs[4].base_AC);
    //exit(1);

    gff_find_chunk_header(man->ds1.gpl, &chunk, GFF_NAME, 1);
    names = malloc(chunk.length);
    num_names = chunk.length / 25;
    gff_read_chunk(man->ds1.gpl, &chunk, names, chunk.length);
    if (amt != chunk.length) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
    */
}

extern int gff_manager_get_item1r(gff_manager_t *man, const int32_t item_idx, ds_item1r_t *item1r) {
    get_item1rs(man);

    if (item_idx < 0 || item_idx >= num_item1rs) {
        goto bounds_error;
    }

    memcpy(item1r, item1rs + item_idx, sizeof(ds_item1r_t));

    return EXIT_SUCCESS;
bounds_error:
    return EXIT_FAILURE;
}

extern int gff_manager_get_name(gff_manager_t *man, const int32_t name_idx, char *buf) {
    get_item1rs(man);
    if (name_idx < 0 || name_idx >= num_names) { return EXIT_FAILURE; }
    //return names + 25 * name_idx;
    memcpy(buf, names + 25 * name_idx, 25);
    return EXIT_SUCCESS;
}
