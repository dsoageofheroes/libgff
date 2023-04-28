#ifndef GFF_IMAGE_H
#define GFF_IMAGE_H

#include "common.h"

typedef struct gff_image_entry_s {
    uint16_t frame_num;
    uint32_t data_len;
    char data[];
} gff_image_entry_t;

typedef struct gff_frame_entry_s {
    uint16_t width, height;
    char rgba[];
} gff_frame_entry_t;

typedef struct gff_frame_info_s {
    uint16_t w, h;
} gff_frame_info_t;

/* Under Test */
extern int             gff_palettes_read(gff_file_t *f);
extern int             gff_palettes_read_type(gff_file_t *f, int type_id);
extern int             gff_palettes_free(gff_file_t *f);
extern int             gff_get_frame_count(gff_file_t *f, int type_id, int res_id);
extern int             gff_frame_info(gff_file_t *f, int type_id, int res_id, int frame_id, gff_frame_info_t *info);
extern unsigned char*  gff_get_frame_rgba_palette(gff_file_t *f, int type_id, int res_id, int frame_id, const gff_palette_t *pal);
extern unsigned char*  gff_get_frame_rgba_palette_img(gff_image_entry_t *img, int frame_id, const gff_palette_t *pal);
extern unsigned char*  gff_create_font_rgba(gff_file_t *f, int c, int fg_color, int bg_color);

/* Not tested */
extern int             gff_image_is_png(gff_file_t *f, int type_id, int res_id, int frame_id);
extern int             gff_image_load_png(const int gff_index, int type_id, const uint32_t image_id,
        const int frame_id, uint32_t *w, uint32_t *h, unsigned char **data);

/* Consider removing */
extern void            gff_image_init();
extern gff_palette_t*  gff_create_palettes(gff_file_t *f, unsigned int *len);
extern int             gff_get_number_of_palettes();
extern int             gff_get_frame_width(gff_file_t *f, int type_id, int res_id, int frame_id);
extern int             gff_get_frame_height(gff_file_t *f, int type_id, int res_id, int frame_id);
extern unsigned char*  gff_get_portrait(unsigned char* bmp_table, unsigned int *width, unsigned int *height);

/* Migrate to gff manager */
extern unsigned char*  gff_get_frame_rgba_with_palette(gff_file_t *f, int type_id, int res_id, int frame_id, int palette_id);

#endif
