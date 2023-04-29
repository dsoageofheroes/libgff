#ifndef GFF_MAP_H
#define GFF_MAP_H

#include <stdint.h>

#include "rdff.h"
#include "scmd.h"
#include "object.h"
#include "common.h"

#define MAP_LOS     (0x80) // Runtime flag to determine if block is in sight.
#define MAP_BLOCK   (0x40) // This tile is blocked (you can't move into it.)
#define MAP_ACTOR   (0x20) // If there is actor here (and is blocked.)
#define MAP_DANGER  (0x07) // If it is dangerous and you are not courageous, you are blocked
#define MAP_GPL     (0x04) // ????

#define MAP_ROWS 98
#define MAP_COLUMNS 128

#define MAX_MAP_OBJECTS (2000)

#define VALID_MAP_ROW(row) (row >= 0 && row < MAP_ROWS)
#define VALID_MAP_COLUMN(col) (col >= 0 && col < MAP_COLUMNS)
#define HAS_MAP_DATA(gff_index) (gff_index >= 0 && gff_file < NUM_FILES && open_files[gff_index].map)
#define OBJECT_PRIORITY_MASK (0x07)
#define OBJECT_EXISTS        (0x08)
#define OBJECT_DONT_DRAW     (0x10)
#define OBJECT_ONE_OBJECT    (0x20) // This object is aliased!
#define OBJECT_REDRAW        (0x40)
#define OBJECT_XMIRROR       (0x80) // Need to flip x axis.

#define GM_ANIMATING (0x01)
#define GM_STATIC    (0x02)

typedef struct gff_region_object_s {
    uint16_t region_id, etab_id;
    gff_ojff_t ojff;
    gff_scmd_t *scmd;
} gff_region_object_t;

typedef struct gff_region_s {
    uint8_t flags[MAP_ROWS][MAP_COLUMNS];
    uint8_t tiles[MAP_ROWS][MAP_COLUMNS];
    uint32_t *tile_ids;
    gff_etab_object_t *etab;
    uint32_t num_objects, id;
    gff_region_object_t *objs;
} gff_region_t;

// The small 'n' means next. IE: nbmp_idx = next bitmap index.
typedef struct _gff_script_cmd_t {
    uint8_t nbmp_idx;     // 0-254 = valid, 255 invalid.
    uint8_t nbmp_delay;   // 0-255 = valid
    uint8_t flags;
    int8_t  nxoffset;
    int8_t  nyoffset;
    int8_t  npxoffset;
    int8_t  npyoffset;
    uint8_t sound_index;
} gff_script_cmd_t;

#define MAX_SCRIPTS (240)
#define MAX_SCRIPT_JUMPS (36)
#define NULL_OBJECT (9999) // why 9999?  I dunno!

typedef struct _gff_script_jump_t {
    uint16_t jump_ids[MAX_SCRIPTS][MAX_SCRIPT_JUMPS];
} gff_script_jump_t;

typedef struct _gff_script_entry_t {
    uint8_t use_count;
    uint16_t chunk_id;
    gff_script_cmd_t *cmds;
    int16_t size;
} gff_script_entry_t;

extern gff_script_entry_t script_entry_table[MAX_SCRIPTS];

// Types of Sol Oscuro objects
enum {
    SO_EMPTY = 0, // Empty: free to overide!
    SO_DS1_COMBAT,
    SO_DS1_ITEM
};

// Tested
extern gff_region_t*  gff_region_load(gff_file_t *f);
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
