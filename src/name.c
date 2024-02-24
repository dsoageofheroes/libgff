#include "gff/manager.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"

#include <string.h>

extern int gff_name_get_random(gff_manager_t *man, char *name, const size_t len, int race, int gender) {
// FIXME - For DS2/DSO, there may be new random names (I don't think there are?)
// Hard-coding these unless we can come up with a better way of figuring out
// which names are Female/Thri-Kreen
    gff_chunk_header_t chunk;
    uint32_t res_ids[1<<12];
    uint32_t num_ids;
    int name_list_start  = race == RACE_THRIKREEN ? 52 : gender == GENDER_FEMALE ? 33 : 0;
    int name_list_length = race == RACE_THRIKREEN ? 7  : gender == GENDER_FEMALE ? 17 : 32;
    int chosen_name = (rand() % (name_list_length + 1)) + name_list_start;

    //memset(&name, 0, len);

    // Name resource length is hard-coded now
    if (gff_get_resource_ids(man->ds1.resource, GFF_TEXT, res_ids, &num_ids)) {
        goto resource_error;
    }
    if (gff_find_chunk_header(man->ds1.resource, &chunk, GFF_TEXT, res_ids[chosen_name])) {
        goto no_header_error;
    }
    if (gff_read_chunk(man->ds1.resource, &chunk, name, len)) {
        goto read_chunk_error;
    }

    name[len-1] = '\0';

    return EXIT_SUCCESS;
read_chunk_error:
no_header_error:
resource_error:
    return EXIT_FAILURE;
}
