#include "gff/rdff.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/region.h"

#include <stdlib.h>
#include <string.h>

#define RDFF_MAX (1<<12)
static int fill_object(char *data, gff_rdff_header_t *entry, int16_t id, gff_object_t *obj);

extern int gff_rdff_load(gff_file_t *f, int res_id, gff_rdff_t *rdff) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(f, &chunk, GFF_RDFF, res_id)) {
        goto header_read_error;
    }

    if (chunk.length > sizeof(gff_rdff_t)) {
        goto buffer_error;
    }

    if (gff_read_chunk(f, &chunk, rdff, chunk.length) < chunk.length) {
        goto read_error;
    }

    return EXIT_SUCCESS;
buffer_error:
    printf ("chunk.length (%d) greater than max (%d)!\n", chunk.length, RDFF_MAX);
read_error:
header_read_error:
    return EXIT_FAILURE;
}

extern int gff_rdff_to_object(gff_rdff_t *rdff, gff_object_t *obj) {
    int status = EXIT_FAILURE;
    char *tmp = NULL;
    gff_rdff_header_t *entry = &(rdff->header);
    char *data = (char*)entry;

    while (entry->load_action != -1) {
        //printf ("proc = %d, bocknum = %d, type = %d, index = %d, from = %d, len = %d\n",
                //entry->load_action, entry->blocknum, entry->type, entry->index, entry->from, entry->len);
        switch(entry->load_action) {
            case RDFF_OBJECT:
                //printf("OBJECT\n");
                data += sizeof(gff_rdff_header_t);
                status = fill_object((char*) data, entry, -1, obj);
                goto exit;
            case RDFF_CONTAINER:
                printf("CONTAINER\n");
                break;
            case RDFF_DATA:
                //printf("DATA\n");
                break;
            case RDFF_NEXT:
                printf("NEXT\n");
                break;
            case RDFF_END:
                printf("END\n");
                break;
        }
    
        // Iterate to the next object
        tmp = (char*)entry;
        tmp += sizeof(gff_rdff_header_t);
        tmp += entry->len;
        entry = (gff_rdff_header_t *)tmp;
    }

exit:
    return status;
}

static int fill_object(char *data, gff_rdff_header_t *entry, int16_t id, gff_object_t *obj) {
    int i;

    obj->type = SO_EMPTY;
    switch(entry->type) {
        case GFF_COMBAT_OBJECT:
            obj->type = SO_DS1_COMBAT;
            memcpy(&(obj->data.ds1_combat), (ds1_combat_t*) data, sizeof(ds1_combat_t) - 16); // Don't copy the name over!
            ds1_combat_t *combat = &(obj->data.ds1_combat);
            for (i = 0; i < 17 && ((ds1_combat_t*)data)->name[i]; i++) {
                combat->name[i] = ((ds1_combat_t*)data)->name[i];
            }
            // Force the name to be null-terminated.
            i = i >= 17 ? 16 : i;
            combat->name[i] = '\0';
            //printf("name = %s\n", combat->name);
            /*
            printf("\n[%d]:", 0);
            for (int i = 0; i < sizeof(ds1_combat_t); i++) {
                printf("%d, ", ((char*)combat)[i]);
                if ((i % 16) == 15) { printf("\n[%d]:", i+1); }
            }
            printf("\n");
            printf("ac = %d, movement = %d, thac0 = %d, hp = %d, psi = %d"
                "special attack = %d\n", 
                combat->ac, combat->move, combat->thac0, combat->hp, combat->psi,
                combat->special_attack);
            */
            combat->char_index = NULL_OBJECT;
            combat->ready_item_index = NULL_OBJECT;
            combat->weapon_index = NULL_OBJECT;
            combat->pack_index = NULL_OBJECT;
            // Always start at least with an OK combat status.
            if (combat->status == 0) {
                //combat->status = COMBAT_STATUS_OK;
            }
            break;
        case GFF_ITEM_OBJECT:
            obj->type = SO_DS1_ITEM;
            ds1_item_t *item = &(obj->data.ds1_item);
            memcpy(item, (ds1_item_t*) data, sizeof(ds1_item_t));
            /*
            printf("id = %d\n", item->id);
            printf("quantity = %d\n", item->quantity);
            printf("next = %d\n", item->next);
            printf("value = %d\n", item->value);
            printf("pack_index = %d\n", item->item_index);
            printf("icon = %d\n", item->icon);
            printf("charges = %d\n", item->charges);
            printf("special = %d\n", item->special);
            printf("slot = %d\n", item->slot);
            printf("name_index = %d\n", item->name_index);
            printf("adds = %d\n", item->adds);
            disk_object_t* diskobject = gff_get_object(item->id);
            printf("bmpid = %d\n", diskobject->bmp_id);
            printf("script_id = %d\n", diskobject->script_id);
            */
            break;
        case GFF_CHAR_OBJECT:
            printf("CHARACTER!!!!!!!!!!!!!!!!!!!!\n");
            exit(1);
            goto char_error;
            break;
        case GFF_MINI_OBJECT:
            printf("MINI\n");
            goto mini_error;
            break;
        default:
            printf("-----------UNKNOWN ITEM TYPE: %d\n", entry->type);
            goto unknown_error;
            break;
    }

    return EXIT_SUCCESS;
unknown_error:
mini_error:
char_error:
    return EXIT_FAILURE;
}
