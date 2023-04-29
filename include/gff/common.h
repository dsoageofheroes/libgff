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
    unsigned short r, g, b;
} gff_color_t;

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

#endif
