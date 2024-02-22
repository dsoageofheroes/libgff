#ifndef GFF_GUI_H
#define GFF_GUI_H

#include <stdint.h>

#include "common.h"

typedef struct gff_resource_header_s {
    uint32_t type;
    uint32_t len;
    uint32_t id;
} gff_resource_header_t;
typedef struct gff_accl_entry_s {

    uint8_t  flags;
    uint16_t event;
    uint16_t user_id;
} __attribute__ ((__packed__)) gff_accl_entry_t;

typedef struct gff_accl_s {
    gff_resource_header_t rh;
    uint16_t              count;
    gff_accl_entry_t      entries[];
} __attribute__ ((__packed__)) gff_accl_t;

typedef struct gff_rect_s {
    int16_t xmin, ymin;
    int16_t xmax, ymax;
} gff_rect_t;

typedef struct gff_gui_item_s {
    int8_t data[4];
    uint32_t type;
    uint32_t id;
    gff_rect_t init_bounds;
    gff_rect_t item_bounds;
    uint16_t flags;
} __attribute__ ((__packed__)) gff_gui_item_t;

typedef struct gff_gui_region_s {
    uint16_t depth;
    gff_rect_t bounds;
    gff_rect_t regions[16];
} gff_gui_region_t;

typedef struct gff_frame_s {
    uint16_t flags;
    uint16_t type;
    gff_rect_t bounds;
    gff_rect_t initbounds;
    gff_rect_t zonebounds;
    uint16_t width;
    uint16_t height;
    uint32_t border_bmp;
    int8_t data[4]; // This may be two bytes.
    uint32_t background_bmp;
    int8_t data1[4]; // This may be two bytes.
    char title[24];
    //char data0[44];
} __attribute__ ((__packed__)) gff_frame_t;

typedef struct gff_window_s {
    gff_resource_header_t rh;
    gff_gui_region_t region;
    int16_t x;
    int16_t y;
    int8_t data[4];
    uint16_t flags;
    short data1;
    gff_frame_t  frame;
    int16_t offsetx;
    int16_t offsety;
    int8_t data2[4];
    uint8_t titleLen;
    uint16_t itemCount;
    uint32_t mem; // total mem needed
} __attribute__ ((__packed__)) gff_window_t;

typedef struct gff_app_frame_s {
    gff_resource_header_t rh;
    gff_frame_t frame;
    uint8_t data[4];
    int16_t event_filter;
    uint8_t data1[4];
    uint8_t data2[4];
    uint8_t data3[4];
    int16_t snapMode;
    int16_t snapOriginX;
    int16_t snapOriginY;
    int16_t snapSizeX;
    int16_t snapSizeY;
    uint8_t data4[4];
} __attribute__ ((__packed__)) gff_app_frame_t;

typedef struct gff_button_s {
    gff_resource_header_t rh;
    gff_frame_t frame;
    uint8_t pad0[4];
    uint16_t flags;
    uint16_t userid;
    int16_t  iconx, icony;
    int16_t  textx, texty;
    uint32_t icon_id;
    uint8_t pad1[4];
    uint8_t key;
    uint8_t textlen;
    char    text[64];
} __attribute__ ((__packed__)) gff_button_t;

typedef struct gff_ebox_s {
    gff_resource_header_t rh;
    uint8_t   pad0[4];
    uint16_t  max_lines, styles, runs, size, user_id;
    gff_frame_t frame;
    //uint8_t   data[28]; // gff_ebox_t should be 168 bytes
    uint8_t   data[70]; // gff_ebox_t should be 168 bytes
    //gff_rect_t rect;
} __attribute__ ((__packed__)) gff_ebox_t;

#define MAX_CHARS (128)

typedef struct gff_font_s {
    uint16_t num;
    uint16_t height;
    uint16_t background_color;
    uint16_t flags;
    uint8_t  colors[256];
    uint16_t char_offset[];
} gff_font_t;

typedef struct _ds_char_s {
    uint16_t width;
    uint8_t  data[0];
} gff_char_t;

extern int gff_read_font(gff_file_t *f, int res_id, gff_font_t **font);
extern int gff_read_ebox(gff_file_t *f, int res_id, gff_ebox_t *ebox);
extern int gff_read_window(gff_file_t *f, int res_id, gff_window_t **win);
extern int gff_read_button(gff_file_t *f, int res_id, gff_button_t *button);
extern int gff_read_frame(gff_file_t *f, int res_id, gff_frame_t *frame);
extern int gff_read_raw_pal(gff_file_t *f, int res_id, gff_raw_palette_t *pal);

#endif
