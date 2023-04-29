#ifndef GFF_MANAGER_H
#define GFF_MANAGER_H

#include <stdlib.h>
#include <stdint.h>

#include "gff/common.h"
#include "gff/gui.h"
#include "gff/manager.h"
#include "gff/object.h"
#include "gff/status.h"
#include "gff/region.h"

#define DS1_MAX_REGIONS (50)

typedef struct gff_ds1_manager_s {
    gff_file_t *resource, *segobjex, *gpl, *cine;
    gff_file_t *regions[DS1_MAX_REGIONS];
    gff_file_t *wild_region;
} gff_ds1_manager_t;

typedef struct gff_manager_s {
    gff_ds1_manager_t ds1;
} gff_manager_t;

extern gff_manager_t* gff_manager_create();
extern int            gff_manager_free(gff_manager_t *man);

extern int            gff_manager_load_ds1(gff_manager_t *man, const char *path);
extern int            gff_manager_load_region_objects(gff_manager_t *man, gff_region_t *reg);
extern int            gff_manager_create_ds1_region_object(gff_manager_t *man, int region, int etab_id, gff_region_object_t *obj);
extern int            gff_manager_read_window(gff_ds1_manager_t *man, int res_id, gff_window_t **win);
/*
extern gff_status_t gff_gpl_manager_init();
extern gff_status_t gff_gpl_manager_cleanup();
extern gff_status_t gff_gpl_lua_execute_script(size_t file, size_t addr, uint8_t is_mas);
extern gff_status_t gff_gpl_execute_string(const char *str);
extern gff_status_t gff_gpl_lua_load_all_scripts();
extern gff_status_t gff_gpl_lua_debug();
extern gff_status_t gff_gpl_push_context();
extern gff_status_t gff_gpl_pop_context();
extern gff_status_t gff_gpl_set_exit();
extern gff_status_t gff_gpl_in_exit();
*/

#endif
