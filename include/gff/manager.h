#ifndef GFF_MANAGER_H
#define GFF_MANAGER_H

#include <stdlib.h>
#include <stdint.h>

#include "gff/common.h"
#include "gff/gui.h"
#include "gff/object.h"
#include "gff/status.h"
#include "gff/region.h"

#define DS1_MAX_REGIONS (50)

typedef struct gff_ds1_manager_s {
    gff_file_t *resource, *segobjex, *gpl, *cine, *charsave;
    gff_file_t *regions[DS1_MAX_REGIONS];
    gff_file_t *wild_region;
} gff_ds1_manager_t;

typedef struct gff_manager_s {
    gff_ds1_manager_t ds1;
} gff_manager_t;

extern gff_manager_t* gff_manager_create();
extern int            gff_manager_free(gff_manager_t *man);

extern int             gff_manager_load_ds1(gff_manager_t *man, const char *path);
extern int             gff_manager_load_region_objects(gff_manager_t *man, gff_region_t *reg);
extern int             gff_manager_create_ds1_region_object(gff_manager_t *man, int region, int etab_id, gff_region_object_t *obj);
extern int             gff_manager_read_window(gff_ds1_manager_t *man, int res_id, gff_window_t **win);
extern int             gff_manager_font_load(gff_manager_t *man, uint8_t **data, int32_t *w, int32_t *h, const uint32_t fg_color, const uint32_t bg_color);
extern gff_game_type_t gff_manager_game_type(gff_manager_t *man);
extern int             gff_item_load(gff_manager_t *man, ds1_item_t *item, int32_t id);
extern int             gff_manager_get_item1r(gff_manager_t *man, const int32_t item_idx, ds_item1r_t *item1r);
extern int             gff_manager_get_name(gff_manager_t *man, const int32_t name_idx, char *buf);
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
