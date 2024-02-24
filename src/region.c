#include <stdlib.h>
#include <string.h>
#include "gff/region.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/gpl.h"
//#include "region.h"
//#include "combat.h"

#define RDFF_MAX (1<<12)
#define GMAP_MAX (MAP_ROWS * MAP_COLUMNS)

extern int gff_read_etab(gff_file_t *gff, int res_id, gff_etab_object_t **etab, uint32_t *num_etabs) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_ETAB, res_id) == EXIT_FAILURE) {
        goto dne;
    }

    *etab = malloc(chunk.length);

    if (!*etab) {
        error ("unable to malloc for entry table!\n");
        exit(1);
    }

    gff_read_chunk(gff, &chunk, *etab, chunk.length);
    *num_etabs = chunk.length / sizeof(gff_etab_object_t);

    return EXIT_SUCCESS;
dne:
    return EXIT_FAILURE;
}

extern int gff_read_global_flags(gff_file_t *gff, int res_id, uint8_t flags[MAP_ROWS][MAP_COLUMNS]) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_GMAP, res_id)) {
        goto chunk_error;
    }

    if (chunk.length > GMAP_MAX) {
        error ("chunk.length (%d) is grater that GMAP_MAX(%d)\n", chunk.length, GMAP_MAX);
        goto chunk_length_error;
    }

    if (!gff_read_chunk(gff, &chunk, flags, chunk.length)) {
        error ("Unable to read GFF_GMAP chunk!\n");
        goto region_read_error;
    }

    return EXIT_SUCCESS;
region_read_error:
chunk_length_error:
chunk_error:
    return EXIT_FAILURE;
}

extern int gff_read_region_flags(gff_file_t *gff, int res_id, uint8_t flags[MAP_ROWS][MAP_COLUMNS]) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_RMAP, res_id)) {
        goto chunk_error;
    }

    if (chunk.length > GMAP_MAX) {
        error ("chunk.length (%d) is grater that GMAP_MAX(%d)\n", chunk.length, GMAP_MAX);
        goto chunk_length_error;
    }

    if (!gff_read_chunk(gff, &chunk, flags, chunk.length)) {
        error ("Unable to read GFF_RMAP chunk!\n");
        goto region_read_error;
    }

    return EXIT_SUCCESS;
region_read_error:
chunk_length_error:
chunk_error:
    return EXIT_FAILURE;
}

extern int gff_region_get_num_objects(gff_file_t *f, uint32_t *amt) {
    if (!f) { goto null_err; }

    if (f->num_objects > 0) {
        *amt = f->num_objects;
    }

    if (!f->entry_table) {
        gff_chunk_header_t chunk;
        if (gff_find_chunk_header(f, &chunk, GFF_ETAB, f->id) == EXIT_FAILURE) {
            goto dne;
        }
        f->entry_table = malloc(chunk.length);
        if (!f->entry_table) {
            error ("unable to malloc for entry table!\n");
            exit(1);
        }
        gff_read_chunk(f, &chunk, f->entry_table, chunk.length);
        f->num_objects = chunk.length / sizeof(gff_etab_object_t);
    }

    *amt = f->num_objects;
    return EXIT_SUCCESS;
dne:
null_err:
    return EXIT_FAILURE;
}

/*
extern int gff_region_get_object_frame_count(gff_file_t *f, int res_id, int obj_id) {
    int num_objects = gff_region_get_num_objects(f, res_id);
    disk_object_t disk_object;

    if (!f || obj_id < 0 || obj_id >= num_objects) {
        return -1;
    }

    gff_region_object_t *entry_table = f->entry_table;
    if (entry_table == NULL) { return EXIT_FAILURE; }

    gff_read_object(entry_table[obj_id].index, &disk_object);

    //f->status = GFF_NOT_IMPLEMENTED;
    //return gff_get_frame_count(OBJEX_GFF_INDEX, GFF_BMP, disk_object.bmp_id);
    return gff_get_frame_count(f, GFF_BMP, disk_object.bmp_id);
}
*/

/*
int gff_region_fill_scmd_info(gff_dude_t *dude, int gff_index, int res_id, int obj_id, int scmd_index) {
    int num_objects = gff_region_get_num_objects(gff_index, res_id);
    disk_object_t disk_object;

    if (!dude || gff_index < 0 || gff_index >= NUM_FILES || obj_id < 0 || obj_id >= num_objects) {
        return 0;
    }

    gff_region_object_t *entry_table = open_files[gff_index].entry_table;
    if (entry_table == NULL) { return 0; }
    gff_read_object(entry_table[obj_id].index, &disk_object);

    dude->anim.scmd_info.gff_idx = OBJEX_GFF_INDEX;
    dude->anim.scmd_info.res_id = disk_object.script_id;
    dude->anim.scmd_info.index = scmd_index;

    return 1;
}

extern int gff_region_load_scmd(gff_entity_t *dude) {
    if (!dude) { return 0; }

    //printf("scmd loading: %d, %d, %d\n", dude->anim.scmd_info.gff_idx, dude->anim.scmd_info.res_id, dude->anim.scmd_info.index);
    return (dude->anim.scmd = gff_scmd_get(dude->anim.scmd_info.gff_idx,
            dude->anim.scmd_info.res_id, dude->anim.scmd_info.index)) != NULL;
}
*/

extern unsigned char* gff_region_get_object_bmp_pal(gff_file_t *f, int res_id, int obj_id, int *w, int *h, int frame_id,
        int palette_id) {
    /*
    int num_objects = gff_region_get_num_objects(f, res_id);
    //gff_ojff_t disk_object;

    if (!f || obj_id < 0 || obj_id >= num_objects) {
        return NULL;
    }

    gff_region_object_t *entry_table = f->entry_table;
    if (entry_table == NULL) { return NULL; }
    */

    /*
    gff_read_object(entry_table[obj_id].index, &disk_object);
    int num_frames = gff_get_frame_count(OBJEX_GFF_INDEX, GFF_BMP, disk_object.bmp_id);
    //printf("num_frames = %d\n", num_frames);
    //printf("frame_id = %d\n", frame_id);
    if (frame_id >= num_frames) {
        printf("ERROR: requesting a frame that out of range!\n");
        return NULL;
    }
    *w = gff_get_frame_width(OBJEX_GFF_INDEX, GFF_BMP, disk_object.bmp_id, frame_id);
    *h = gff_get_frame_height(OBJEX_GFF_INDEX, GFF_BMP, disk_object.bmp_id, frame_id);
    return gff_get_frame_rgba_with_palette(OBJEX_GFF_INDEX, GFF_BMP, disk_object.bmp_id, frame_id, palette_id);
    */
    f->status = GFF_NOT_IMPLEMENTED;
    return NULL;
}

extern unsigned char* gff_region_get_object_bmp(gff_file_t *f, int res_id, int obj_id, int *w, int *h, int frame_id) {
    return gff_region_get_object_bmp_pal(f, res_id, obj_id, w, h, frame_id, -1);
}

/*
extern uint16_t gff_region_get_object_location(gff_file_t *f, int res_id, int obj_id, uint16_t *x, uint16_t *y, uint8_t *z) {
    int num_objects = gff_region_get_num_objects(f, res_id);
    disk_object_t disk_object;

    if (!f || obj_id < 0 || obj_id >= num_objects) {
        *x = *y = 0xFFFF;
        return 0;
    }

    gff_region_object_t *entry = f->entry_table;
    if (entry == NULL) { *x = *y = 0xFFFF; return 0; }
    gff_read_object(entry[obj_id].index, &disk_object);
    //if (disk_object->script_id > 0) {
        //dsl_scmd_print(OBJEX_GFF_INDEX, disk_object->script_id);
    //}
    entry += obj_id;
    *x = entry->xpos;
    *y = entry->ypos;
    *x = entry->xpos - disk_object.xoffset;
    *y = entry->ypos - disk_object.yoffset;
    *z = entry->zpos;

    return disk_object.flags;
}
*/

gff_object_t* gff_create_object(char *data, gff_rdff_header_t *entry, int16_t id) {
    int i;
    gff_object_t* obj = calloc(1, sizeof(gff_object_t)); // calloc zeroizes.
    if (!obj) { return NULL; } // Calloc failed.
    //int16_t *objectHeader;
    //data += 9;
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
            break;
        case GFF_MINI_OBJECT:
            printf("MINI\n");
            break;
        default:
            printf("-----------UNKNOWN ITEM TYPE: %d\n", entry->type);
            break;
    }

    return obj;
}

static int load_region_flags(gff_file_t *f, gff_region_t *region) {
    uint32_t len;
    unsigned int *gmap_ids = gff_get_id_list(f, GFF_GMAP, &len);
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(f, &chunk, GFF_GMAP, gmap_ids[0])) {
        goto chunk_error;
    }

    if (chunk.length > GMAP_MAX) {
        error ("chunk.length (%d) is grater that GMAP_MAX(%d)\n", chunk.length, GMAP_MAX);
        goto chunk_length_error;
    }

    if (!gff_read_chunk(f, &chunk, region->flags, chunk.length)) {
        error ("Unable to read GFF_GMAP chunk!\n");
        goto region_read_error;
    }

    region->id = f->id;

    free(gmap_ids);
    return EXIT_SUCCESS;

region_read_error:
chunk_length_error:
chunk_error:
    free(gmap_ids);
    return EXIT_FAILURE;
}

static int load_tiles(gff_file_t *f, gff_region_t *region) {
    uint32_t len;
    unsigned int *rmap_ids = gff_get_id_list(f, GFF_RMAP, &len);
    unsigned char *data;

    if (rmap_ids == NULL) {
        goto dne;
    }
    
    gff_chunk_header_t chunk;
    if (gff_find_chunk_header(f, &chunk, GFF_RMAP, rmap_ids[0])) {
        goto no_rmap;
    }

    data = malloc(chunk.length);
    if (!data) {
        error ("unable to allocate data for rmap\n");
        goto mem_error;
    }

    if (!gff_read_chunk(f, &chunk, data, chunk.length)) {
        error ("Unable to read RMAP!\n");
        goto read_failure;
    }

    if (gff_get_resource_length(f, GFF_TILE, &len)) {
        goto get_length_error;
    }
    memcpy(region->tiles, data, chunk.length);

    free(data);
    region->tile_ids = gff_get_id_list(f, GFF_TILE, &len);

    free(rmap_ids);
    return EXIT_SUCCESS;
get_length_error:
read_failure:
    free(data);
mem_error:
no_rmap:
    free(rmap_ids);
dne:
    return EXIT_FAILURE;
}

static int load_region_etab(gff_file_t *f, gff_region_t *region) {
    uint32_t len;
    unsigned int *gmap_ids = gff_get_id_list(f, GFF_GMAP, &len);

    if (!f) { goto null_err; }

    gff_chunk_header_t chunk;
    if (gff_find_chunk_header(f, &chunk, GFF_ETAB, gmap_ids[0]) == EXIT_FAILURE) {
        goto dne;
    }

    region->etab = malloc(chunk.length);
    if (!region->etab) {
        error ("unable to malloc for entry table!\n");
        goto memory_error;
    }

    if (!gff_read_chunk(f, &chunk, region->etab, chunk.length)) {
        error ("unable to read entry table!\n");
        goto read_error;
    }
    region->num_objects = chunk.length / sizeof(gff_etab_object_t);

    free(gmap_ids);
    return EXIT_SUCCESS;
read_error:
memory_error:
dne:
null_err:
    free(gmap_ids);
    return EXIT_FAILURE;
}

extern gff_region_t* gff_region_load(gff_file_t *f) {
    gff_region_t *region;

    if (!f) { return NULL; }

    region = (gff_region_t*) malloc(sizeof(gff_region_t));
    if (!region) { goto memory_error; }

    if (load_region_etab(f, region)) {
        goto etab_error;
    }

    if (load_region_flags(f, region)) {
        goto flag_error;
    }

    if (load_tiles(f, region)) {
        goto tile_error;
    }

    return region;

etab_error:
flag_error:
tile_error:
    gff_region_free(region);
memory_error:
    return NULL;
}

extern gff_object_t* gff_object_inspect(gff_file_t *f, int res_id) {
    //gff_object_t *ret = NULL;
    //char *tmp = NULL;
    //gff_rdff_header_t rdff_buf[RDFF_MAX];

    //gff_rdff_header_t *rdff = rdff_buf;
    gff_chunk_header_t chunk;
    gff_find_chunk_header(f, &chunk, GFF_RDFF, res_id);

    if (chunk.length > RDFF_MAX * sizeof(gff_rdff_header_t)) {
        error ("chunk.length (%d) greater than max (%d)!\n", chunk.length, RDFF_MAX);
        exit(1);
    }

    f->status = GFF_NOT_IMPLEMENTED;
    return NULL;

    /*
    gff_idx = OBJEX_GFF_INDEX;
    gff_read_chunk(gff_idx, &chunk, rdff, chunk.length);
    rdff_disk_object_t *entry = rdff;
    char *data = (char*)entry;

    while (entry->load_action != -1) {
        //printf ("proc = %d, bocknum = %d, type = %d, index = %d, from = %d, len = %d\n",
                //entry->load_action, entry->blocknum, entry->type, entry->index, entry->from, entry->len);
        switch(entry->load_action) {
            case RDFF_OBJECT:
                //printf("OBJECT\n");
                data += sizeof(rdff_disk_object_t);
                ret = gff_create_object((char*) data, entry, -1);
                return ret;
                break;
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
        tmp += sizeof(rdff_disk_object_t);
        tmp += entry->len;
        entry = (rdff_disk_object_t *)tmp;
    }

    return ret;
    */
}

extern int gff_region_object_free(gff_region_object_t *obj) {
    if (!obj) { return EXIT_FAILURE; }

    if (obj->scmd) {
        free(obj->scmd);
    }

    return EXIT_SUCCESS;
}

extern int gff_region_free(gff_region_t *region) {
    if (!region) { return EXIT_FAILURE; }

    if (region->objs) {
        for (int i = 0; i < region->num_objects; i++) {
            gff_region_object_free(region->objs + i);
        }
        free(region->objs);
        region->objs = NULL;
    }

    if (region->tile_ids) {
        free(region->tile_ids);
        region->tile_ids = NULL;
    }

    if (region->etab) {
        free(region->etab);
        region->etab = NULL;
    }

    free(region);

    return EXIT_SUCCESS;
}
