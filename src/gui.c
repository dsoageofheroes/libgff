#include "gff/gui.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"

#include <stdlib.h>

/*
static int load_frame_from_gff(const int res_id, gff_frame_t *frame) {
    char buf[4096];
    gff_app_frame_t *gff_frame;

    if (!frame) { return 0; }

    memset(frame, 0x0, sizeof(gff_frame_t));
    frame->spr = SPRITE_ERROR;
    gff_chunk_header_t chunk = gff_find_chunk_header(RESOURCE_GFF_INDEX, GFF_APFM, res_id);
    if (!gff_read_chunk(RESOURCE_GFF_INDEX, &chunk, buf, 4096)) {
        return 0;
    }

    gff_frame = (gff_app_frame_t*)buf;
    frame->gff_id = gff_frame->rh.id;
    frame->base_width = gff_frame->frame.width;
    frame->base_height = gff_frame->frame.height;
    frame->event = gff_frame->event_filter;
    //frame->icon_id = gff_frame->icon_id;
    //frame->base_width = gff_frame->frame.width;
    //frame->base_height = gff_frame->frame.height;

    //render_entry_as_image(gff_idx, GFF_ICON, button->icon_id, open_files[pal_idx].pals->palettes, 320, 92);

    return 1;
}
*/

extern int gff_read_font(gff_file_t *f, int res_id, gff_font_t **font) {
    return 
        gff_read_raw_allocate(f, GFF_FONT, res_id, (char**)font)
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

extern int gff_read_ebox(gff_file_t *f, int res_id, gff_ebox_t *ebox) {
    return 
        gff_read_raw(f, GFF_EBOX, res_id, (char*)ebox, sizeof(gff_ebox_t)) == sizeof(gff_ebox_t)
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

extern int gff_read_window(gff_file_t *f, int res_id, gff_window_t **win) {
    return
        gff_read_raw_allocate(f, GFF_WIND, res_id, (char**)win)
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

extern int gff_read_button(gff_file_t *f, int res_id, gff_button_t *button) {
    return 
        gff_read_raw(f, GFF_BUTN, res_id, (char*)button, sizeof(gff_button_t)) >= 110 
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

extern int gff_read_frame(gff_file_t *f, int res_id, gff_frame_t *frame) {
    return 
        gff_read_raw(f, GFF_APFM, res_id, (char*)frame, sizeof(gff_frame_t)) >= sizeof(gff_frame_t)
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}

extern int gff_read_raw_pal(gff_file_t *f, int res_id, gff_raw_palette_t *pal) {
    return 
        gff_read_raw(f, GFF_PAL, res_id, (char*)pal, sizeof(gff_raw_palette_t)) >= sizeof(gff_raw_palette_t)
        ? EXIT_SUCCESS
        : EXIT_FAILURE;
}
