#include "gff/debug.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/manager.h"
#include "gff/region.h"

#include <string.h>
#include <dirent.h>

#define BUF_SIZE (1024)

extern gff_manager_t* gff_manager_create() {
    gff_manager_t *man = malloc(sizeof(gff_manager_t));

    if (!man) {
        goto memory_error;
    }

    memset(man, 0x0, sizeof(gff_manager_t));

    return man;

memory_error:
    return NULL;
}

static int is_gff_filename(const char *str) {
    int slen = strlen (str);

    return slen > 4 
        && (str[slen-1] == 'f' || str[slen-1] == 'F')
        && (str[slen-2] == 'f' || str[slen-2] == 'F')
        && (str[slen-3] == 'g' || str[slen-3] == 'G');
}

static int detect_and_load_core_ds1(gff_manager_t *man, const char *full_path, const char *name) {
    uint32_t region_num = 0;
    gff_file_t *file = NULL;
    gff_file_t **dest = NULL;

    if (!name || !full_path) { return EXIT_FAILURE; }

    if (sscanf(name, "RGN%x.GFF", &region_num)) {
        if (region_num == 255) {
            dest = &(man->ds1.wild_region);
        } else {
            dest = &(man->ds1.regions[region_num]);
        }
        debug("loading region %d from %s\n", region_num, full_path);
        goto load_file;
    } 

    if (!strcmp(name, "CINE.GFF")) {
        debug("loading cine from %s\n", full_path);
        dest = &(man->ds1.cine);
        goto load_file;
    }

    if (!strcmp(name, "RESOURCE.GFF")) {
        debug("loading resource from %s\n", full_path);
        dest = &(man->ds1.resource);
        goto load_file;
    }

    if (!strcmp(name, "GPLDATA.GFF")) {
        debug("loading gpl from %s\n", full_path);
        dest = &(man->ds1.gpl);
        goto load_file;
    }

    if (!strcmp(name, "SEGOBJEX.GFF")) {
        debug("loading segobjex from %s\n", full_path);
        dest = &(man->ds1.segobjex);
        goto load_file;
    }

    if (!strcmp(name, "CHARSAVE.GFF")) {
        debug("loading charsave from %s\n", full_path);
        dest = &(man->ds1.charsave);
        goto load_file;
    }

    goto dne;

load_file:
    file = gff_allocate();
    if (!file) {
        goto file_load_error;
    }
    if (gff_open(file, full_path)) {
        goto file_open_error;
    }
    file->id = region_num;
    *dest = file;
    file = NULL;
    return EXIT_SUCCESS;

file_open_error:
    gff_free(file);
file_load_error:
dne:
    return EXIT_FAILURE;
}

extern int gff_manager_load_ds1(gff_manager_t *man, const char *path) {
    DIR *dir;
    struct dirent *ent;
    char buf[BUF_SIZE];

    if (!man || !path) { return EXIT_FAILURE; }

    debug("Loading GFFs from: %s\n", path);
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (is_gff_filename(ent->d_name)) {
                snprintf(buf, BUF_SIZE, "%s/%s", path, ent->d_name);
                detect_and_load_core_ds1(man, buf, ent->d_name);
            }
        }
        closedir (dir);
    } else {
        fprintf(stderr, "Unable to open directory: '%s'\n", path);
        goto dir_failure;
    }

    return EXIT_SUCCESS;

dir_failure:
    return EXIT_FAILURE;
}

#define RDFF_MAX (1<<12)

extern int gff_manager_create_ds1_region_object(gff_manager_t *man, int region, int etab_id, gff_region_object_t *obj) {
    //gff_chunk_header_t chunk;
    uint32_t amt, len;
    int32_t ojff_index;
    gff_file_t *f;

    f = man->ds1.regions[region];
    if (f->num_objects <= 0) {
        if (gff_region_get_num_objects(f, &amt)) {
            goto etab_error;
        }
    }

    if (etab_id < 0 || etab_id >= f->num_objects) {
        goto bounds_error;
    }

    memset(obj, 0x0, sizeof(gff_region_object_t));
    obj->region_id = region;
    obj->etab_id = etab_id;
    ojff_index = f->entry_table[etab_id].index;

    if (gff_ojff_read(man->ds1.segobjex, -ojff_index, &obj->ojff)) {
        debug("could not ready ojff!\n");
        goto ojff_error;
    }

    if (obj->ojff.script_id
            && gff_scmd_read(man->ds1.segobjex, obj->ojff.script_id, &obj->scmd, &len)) {
        error("Unable to load object's scmd!");
        goto scmd_error;
    }

    return EXIT_SUCCESS;
scmd_error:
ojff_error:
bounds_error:
etab_error:
    return EXIT_FAILURE;

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
    return EXIT_FAILURE;
}

extern int gff_manager_load_region_objects(gff_manager_t *man, gff_region_t *reg) {
    if (!man || ! reg) { return EXIT_FAILURE; }

    reg->objs = malloc(sizeof(gff_region_object_t) * reg->num_objects);
    if (!reg->objs) {
        goto obj_mem_error;
    }

    for (int i = 0; i < reg->num_objects; i++) {
        if (gff_manager_create_ds1_region_object(man, reg->id, i, &reg->objs[i])) {
            goto obj_read_error;
        }
    }

    return EXIT_SUCCESS;

obj_read_error:
    // We would need to do some serious cleanup here...
obj_mem_error:
    return EXIT_FAILURE;
}

extern int gff_manager_load_window(gff_ds1_manager_t *man, int res_id, gff_window_t **win) {
    return gff_load_window(man->resource, res_id, win);
}

extern int gff_manager_free(gff_manager_t *man) {
    if (!man) { return EXIT_FAILURE; }

    for (int i = 0; i < DS1_MAX_REGIONS; i++) {
        if (man->ds1.regions[i]) {
            gff_free(man->ds1.regions[i]);
            man->ds1.regions[i] = NULL;
        }
    }

    if (man->ds1.wild_region) {
        gff_free(man->ds1.wild_region);
        man->ds1.wild_region = NULL;
    }

    if (man->ds1.cine) {
        gff_free(man->ds1.cine);
        man->ds1.cine = NULL;
    }

    if (man->ds1.resource) {
        gff_free(man->ds1.resource);
        man->ds1.resource = NULL;
    }

    if (man->ds1.segobjex) {
        gff_free(man->ds1.segobjex);
        man->ds1.segobjex = NULL;
    }

    if (man->ds1.charsave) {
        gff_free(man->ds1.charsave);
        man->ds1.charsave = NULL;
    }

    if (man->ds1.gpl) {
        gff_free(man->ds1.gpl);
        man->ds1.gpl = NULL;
    }

    free(man);

    return EXIT_SUCCESS;
}

extern gff_game_type_t gff_manager_game_type(gff_manager_t *man) {
    /*
    if (open_files[RESOURCE_GFF_INDEX].filename && open_files[RESFLOP_GFF_INDEX].filename) {
        return DARKSUN_2;
    }

    if (open_files[RESOURCE_GFF_INDEX].filename && !open_files[RESFLOP_GFF_INDEX].filename) {
        return DARKSUN_1;
    }

    if (!open_files[RESOURCE_GFF_INDEX].filename && open_files[RESFLOP_GFF_INDEX].filename) {
        return DARKSUN_ONLINE;
    }
    */

    if (man->ds1.resource) {
        return DARKSUN_1;
    }

    return DARKSUN_UNKNOWN;
}

