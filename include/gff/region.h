#ifndef GFF_MAP_H
#define GFF_MAP_H

#include <stdint.h>

#include "rdff.h"
#include "scmd.h"
#include "object.h"
#include "common.h"

extern gff_script_entry_t script_entry_table[MAX_SCRIPTS];

// Tested
extern gff_region_t*  gff_region_load(gff_file_t *f);
extern int            gff_region_read(gff_file_t *f, gff_region_t *reg);
extern int            gff_region_free(gff_region_t *region);
extern int            gff_region_get_num_objects(gff_file_t *f, uint32_t *amt);
extern int            gff_region_object_free(gff_region_object_t *obj);

// Not Tested
extern unsigned char* gff_region_get_object_bmp_pal(gff_file_t *f, int res_id, int obj_id, int *w, int *h, int frame_id,
        int palette_id);
extern unsigned char* gff_region_get_object_bmp(gff_file_t *f, int res_id, int obj_id, int *w, int *h, int frame_id);

//extern int            gff_region_fill_scmd_info(struct gff_entity_s *dude, int gff_index, int res_id, int obj_id, int scmd_index);
//extern int            gff_region_load_scmd(struct gff_entity_s *dude);

// Maybe move to a util directory?
extern gff_object_t*   gff_create_object(char *data, gff_rdff_header_t *entry, int16_t id);

// Should be moved to a resource file
extern uint16_t       gff_region_get_object_location(gff_file_t *f, int res_id, int obj_id, uint16_t *x, uint16_t *y, uint8_t *z);

// Consider removing
extern int            gff_region_get_object_frame_count(gff_file_t *f, int res_id, int obj_id);

#endif
