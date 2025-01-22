#ifndef GFF_COMMON_H
#define GFF_COMMON_H

#include <stdint.h>
#include <stdio.h>

#include "status.h"

#define GFFTYPE(d,c,b,a) \
  (((long)(a)<<24) + ((long)(b)<<16) + ((long)(c)<<8) + (long)(d))

#define GFFVERSION     (0x00030000L)
#define GFFMAXCHUNKMASK  (0x7FFFFFFFL)
#define GFFSEGFLAGMASK   (0x80000000L)
#define NUM_FILES (256)
#define PALETTE_SIZE (256)

typedef struct gff_chunk_header_s {
    int32_t   id;
    uint32_t  location;
    uint32_t  length;
} gff_chunk_header_t;

typedef enum gff_game_type_e {
    DARKSUN_UNKNOWN,
    DARKSUN_1,
    DARKSUN_2,
    DARKSUN_ONLINE,
} gff_game_type_t;

enum {
    RESOURCE_GFF_INDEX,
    OBJEX_GFF_INDEX,
    DSLDATA_GFF_INDEX,
    CHARSAVE_GFF_INDEX,
    DARKSAVE_GFF_INDEX,
    CINE_GFF_INDEX,
    DARKRUN_GFF_INDEX,
    RESFLOP_GFF_INDEX,
    REST_GFF_INDEX
};

typedef struct gff_file_header_s {
    uint32_t identity;
    uint32_t version;
    uint32_t data_location;
    uint32_t toc_location;
    uint32_t toc_length;
    uint32_t file_flags;
    uint32_t data0;
} gff_file_header_t;

typedef struct gff_chunk_list_s {
    uint32_t chunk_type;
    uint32_t chunk_count;
    gff_chunk_header_t chunks[0];
} gff_chunk_list_t;

typedef struct gff_toc_header_s {
    uint32_t types_offset;    // Byte offset from a pointer to itself.
    uint32_t free_list_offset;// Byte offset from &typesOffset
} gff_toc_header_t;

typedef struct gff_seg_ref_entry_s {
    int32_t first_id;
    int32_t consec_chunks;
} gff_seg_ref_entry_t;

typedef struct gff_seg_ref_struct_s {
    int32_t num_entries;
    gff_seg_ref_entry_t entries[0];
} gff_seg_ref_struct_t;

typedef struct gff_seg_loc_entry_s {
    int32_t seg_offset;
    int32_t seg_length;
} gff_seg_loc_entry_t;

typedef struct gff_seg_header_s {
  int32_t seg_count;
  int32_t seg_loc_id;
  gff_seg_ref_struct_t  segRef;
} gff_seg_header_t;

typedef struct seg_header_s {
    int32_t seg_count;
    int32_t seg_loc_id;
    int32_t num_entries;
} seg_header_t;

typedef struct gff_seg_entry_s {
    int32_t first_id;
    int32_t num_chunks;
} gff_seg_entry_t;

typedef struct gff_seg_s {
    seg_header_t header;
    gff_seg_entry_t segs[];
} gff_seg_t;

typedef struct gff_chunk_entry_s {
    uint32_t chunk_type;
    uint32_t chunk_count;
    union {
        gff_chunk_header_t chunks[0];
        gff_seg_t segs;
    };
} gff_chunk_entry_t;

typedef struct gff_color_s {
    uint8_t r, g, b;
} gff_color_t;

typedef struct gff_raw_palette_s {
    gff_color_t color[PALETTE_SIZE];
} gff_raw_palette_t;

typedef struct gff_palette_s {
    gff_color_t color[PALETTE_SIZE];
} gff_palette_t;

typedef struct gff_palettes_s {
    uint16_t len;
    gff_palette_t palettes[];
} gff_palettes_t;

typedef struct gff_etab_object_s {
    uint16_t xpos;
    uint16_t ypos;
    int8_t   zpos;
    uint8_t  flags;
    int16_t  index; // bit 15: item is on disk(segobjx.gff), abs(index) = chunk id!
} gff_etab_object_t;

// This will need to be reworked.
typedef struct gff_file_s {
    gff_status_t status;
    //gff_map_t *map;
    char *filename;
    unsigned int num_palettes;
    size_t start_palette_index;
    int num_objects;
    gff_etab_object_t *entry_table;

    // New work for loading only the headers.
    FILE *file;
    gff_file_header_t header;
    gff_toc_header_t toc;
    uint16_t num_types;
    gff_chunk_entry_t **chunks;
    gff_chunk_entry_t *gffi;
    gff_palettes_t *pals;

    uint32_t id;
} gff_file_t;

typedef struct gff_scmd_s {
    uint8_t bmp_idx; // 0-254 = valid, 255 = none
    uint8_t delay;   // 0-255 = valid, ticks are 96 per second
    uint8_t flags;   // See SCMD flags
    int8_t xoffset; // change in x for new bmp
    int8_t yoffset; // change in x for new bmp
    int8_t xoffsethot; // change in x for new bmp
    int8_t yoffsethot; // change in x for new bmp
    uint8_t soundidx;  // sound index to play
} gff_scmd_t;

typedef struct gff_ojff_s {
    uint16_t flags;
    int16_t  xoffset;
    int16_t  yoffset;
    uint16_t xpos;
    uint16_t ypos;
    int8_t   zpos;
    uint8_t  object_index;
    uint16_t bmp_id;
    uint16_t script_id;
} gff_ojff_t;


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


#endif
