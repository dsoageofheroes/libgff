#ifndef GFF_H
#define GFF_H

#include <stdint.h>
#include <stdio.h>

#include "common.h"

// Need to find location for these:
#define MAX_MONSTERS_PER_REGION (10)

typedef struct _gff_monster_entry_t {
    int16_t id;
    int16_t level;
} gff_monster_entry_t;

typedef struct _gff_monster_region_t {
    int16_t region;
    gff_monster_entry_t monsters[MAX_MONSTERS_PER_REGION];
} gff_monster_region_t;

extern gff_monster_entry_t* gff_load_monster(int region_id, int monster_id);

extern gff_file_t open_files[NUM_FILES];

/* In a test */
extern int                gff_init(gff_file_t *f);
extern gff_file_t*        gff_allocate();
extern int                gff_free(gff_file_t *f);
extern int                gff_open(gff_file_t *f, const char *pathName);
extern unsigned int*      gff_get_id_list(gff_file_t *f, int type_id, uint32_t *len);
extern int                gff_find_chunk_header(gff_file_t *f, gff_chunk_header_t *chunk, int type_id, int res_id);
extern size_t             gff_read_chunk(gff_file_t *f, gff_chunk_header_t *chunk, void *read_buf, const size_t len);
extern size_t             gff_read_raw_bytes(gff_file_t *f, int type_id, int res_id, void *read_buf, const size_t len);
extern int                gff_get_number_of_types(gff_file_t *f);
extern int                gff_get_type_id(gff_file_t *f, int type_index);
extern ssize_t            gff_get_palette_len(gff_file_t *f);
extern ssize_t            gff_get_palette_id(gff_file_t *f, int palette_num);
extern size_t             gff_read_chunk_piece(gff_file_t *f, gff_chunk_header_t *chunk, void *read_buf, const size_t len);
extern int                gff_read_raw(gff_file_t *f, int gff_type, int res_id, char *buf, size_t len);
extern size_t             gff_read_raw_allocate(gff_file_t *f, int gff_type, int res_id, char **buf);
extern int                gff_read_text(gff_file_t *f, int res_id, char *text, size_t len);
extern int                gff_read_rdat(gff_file_t *f, int res_id, char *text, size_t len);
extern int                gff_read_spin(gff_file_t *f, int res_id, char *text, size_t len);

/* Not in a test */
extern int                gff_write_raw_bytes(gff_file_t *f, int type_id, int res_id, const char *path); // DEPRECATED?
extern size_t             gff_write_chunk(gff_file_t *f, const gff_chunk_header_t chunk, const char *path);
extern int                gff_create(const char *pathName);
extern size_t             gff_add_chunk(const int idx, const int type_id, int res_id, char *buf, const size_t len);
extern size_t             gff_add_type(const int idx, const int type_id);

/* Consider making private */
/* Consider removing */
extern int                gff_get_resource_length(gff_file_t *f, int type_id, uint32_t *len);
extern int                gff_get_resource_ids(gff_file_t *f, int type_id, unsigned int *ids, uint32_t *pos);
extern int                gff_find_index(const char *name);
extern void               gff_cleanup();
extern void               gff_close (int gff_file);

/* Probably should be in a gff_manager */
extern void               gff_load_directory(const char *path);
extern int                gff_get_master();
extern const char**       gff_list(size_t *len);

#endif
