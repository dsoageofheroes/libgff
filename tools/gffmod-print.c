#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/image.h>
#include <gff/item.h>
#include <gff/gui.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_gff_frame(gff_frame_t *frame) {
    printf("    flags: %d, ", frame->flags);
    printf("type: %d, ", frame->type);
    printf("width: %d, ", frame->width);
    printf("height: %d\n", frame->height);
    printf("    bounds: { (%d, %d) -> (%d, %d) }\n",
            frame->bounds.xmin,
            frame->bounds.ymin,
            frame->bounds.xmax,
            frame->bounds.xmax);
    printf("    initbounds: { (%d, %d) -> (%d, %d) }\n",
            frame->initbounds.xmin,
            frame->initbounds.ymin,
            frame->initbounds.xmax,
            frame->initbounds.xmax);
    printf("    zonebounds: { (%d, %d) -> (%d, %d) }\n",
            frame->zonebounds.xmin,
            frame->zonebounds.ymin,
            frame->zonebounds.xmax,
            frame->zonebounds.xmax);
    printf("    border_bmp: %d\n", frame->border_bmp);
    printf("    background_bmp: %d\n", frame->background_bmp);
    printf("    title: %s\n", frame->title);
}

static void print_button(gff_file_t *gff, unsigned int id) {
    gff_button_t button;

    gff_read_button(gff, id, &button);
    printf("BUTTON #%d\n", id);
    printf("    header = {type: %d, len: %d, id: %d}\n", button.rh.type, button.rh.len, button.rh.id);
    printf("    icon: %d @ (%d, %d)\n", button.icon_id, button.iconx, button.icony);
    print_gff_frame(&button.frame);
    printf("    flags: %d\n", button.flags);
    printf("    userid: %d\n", button.userid);
    printf("    text: '%s' (%d, %d)\n", button.text, button.textx, button.texty);
    printf("    key: %d\n", button.key);
}


static void print_gffi(gff_file_t *gff, unsigned int id) {
    gff_chunk_entry_t *gffi = gff->chunks[id];

    printf("GFFI[%d]:\n", id);
    if (gffi->chunk_count & GFFSEGFLAGMASK) {
        printf("    seg:    num_entries: %u  seg_count: %u location: %u\n", 
                gffi->segs.header.num_entries,
                gffi->segs.header.seg_count,
                gffi->segs.header.seg_loc_id
              );
        for (int i = 0; i < gffi->segs.header.num_entries; i++) {
            printf("        %d: first_id:%d num_chunks: %d\n",
                    i,
                    gffi->segs.segs[i].first_id,
                    gffi->segs.segs[i].num_chunks);
        }
    } else {
        printf("    chunks: total: %lu\n", gffi->chunk_count & GFFMAXCHUNKMASK);
        uint32_t max = gffi->chunk_count & GFFMAXCHUNKMASK;
        for (int i = 0; i < max; i++) {
            printf("        %d: id: %u loc: %u length: %u\n", i,
                    gffi->chunks[i].id,
                    gffi->chunks[i].location,
                    gffi->chunks[i].length);
        }
    }
}

static void print_text(gff_file_t *gff, unsigned int id) {
    char text[4096];

    if (gff_read_text(gff, id, text, 4096)) {
        printf("Unable to read TEXT @ %d\n", id);
    }

    printf("Text[%d]: '%s'\n", id, text);
}

static void print_name(gff_file_t *gff, unsigned int id) {
    char text[16384];
    uint32_t len;

    if (gff_read_names(gff, id, text, 16384, &len)) {
        printf("Unable to read NAME @ %d\n", id);
    }

    printf("Name #%d: count: %d\n", id, len);

    for (int i = 0; i < len; i++) {
        printf("    %d: '%s'\n", i, text + (25 * i));
    }
}

static void print_it1r(gff_file_t *gff, unsigned int id) {
    ds_item1r_t *item1rs = NULL;
    uint32_t     num_item1rs = 0;
    char         names[16384];
    uint32_t     len;

    if (gff_read_names(gff, id, names, 16384, &len)) {
        printf("Unable to read NAME @ %d\n", id);
    }

    if (gff_read_it1r(gff, id, &item1rs, &num_item1rs)) {
        printf("Unable to return IT1R #%d\n", id);
        return;
    }

    for (int i = 0; i < num_item1rs; i++) {
        printf("IT1R #%d: %s\n", i, names + 25 * i);
        printf("    weapon: {type: %d, damage_type: %d, range: %d}\n",
                item1rs[i].weapon_type,
                item1rs[i].damage_type,
                item1rs[i].range);
        printf("    weight: %d\n", item1rs[i].weight);
        printf("    base_hp: %d\n", item1rs[i].base_hp);
        printf("    material: %d\n", item1rs[i].material);
        printf("    placement: %d\n", item1rs[i].placement);
        printf("    damage: %d x %d D %d\n",
                item1rs[i].num_attacks,
                item1rs[i].dice,
                item1rs[i].sides);
        printf("    mod: %d\n", item1rs[i].mod);
        printf("    flags: %d\n", item1rs[i].flags);
        printf("    class: %d\n", item1rs[i].legal_class);
        printf("    AC: %d\n", item1rs[i].base_AC);
    }

    free(item1rs);
}

static void print_spin(gff_file_t *gff, unsigned int id) {
    char spin[4096];

    if (gff_read_spin(gff, id, spin, 4096)) {
        printf("Unable to read SPIN @ %d\n", id);
    }

    printf("SPIN[%d]: '%s'\n", id, spin);
}

static void print_rdat(gff_file_t *gff, unsigned int id) {
    char rdat[4096];

    if (gff_read_spin(gff, id, rdat, 4096)) {
        printf("Unable to read RDAT @ %d\n", id);
    }

    printf("RDAT[%d]: '%s'\n", id, rdat);
}

static void print_monr(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;
    gff_monster_list_t *mon = NULL;
    int num_monsters;

    gff_read_monster_list(gff, id, &mon);
    if (gff_find_chunk_header(gff, &chunk, GFF_MONR, id) ) {
        printf("Unable to read MONR header\n");
        return;
    }

    printf("monster list #%d is in region %d\n", id, mon->region);

    num_monsters = (chunk.length - sizeof(int16_t)) / sizeof(gff_monster_entry_t);
    for (int i = 0; i < num_monsters; i++) {
        printf("    %d: id: %d level: %d\n", i, mon->monsters[i].id, mon->monsters[i].level);
    }

    printf("Warning: GFF_MONR may not be accurate.\n");

    free(mon);
}

static void print_font(gff_file_t *gff, unsigned int id) {
    gff_font_t *font = NULL;

    if (gff_read_font(gff, id, &font)) {
        printf("Unable to read FONT\n");
        return;
    }

    printf("font %d:\n", id);
    printf("    num: %d\n", font->num);
    printf("    height: %d\n", font->height);
    printf("    background_color: %d\n", font->background_color);
    printf("    flags: 0x%x\n", font->flags);
    //uint8_t  colors[256];
    //uint16_t char_offset[];
    printf("Need to complete font...\n");

    free(font);
}

static void print_merr(gff_file_t *gff, unsigned int id) {
    char merr[4096];

    if (gff_read_merr(gff, id, merr, 4096)) {
        printf("Unable to read MERR @ %d\n", id);
    }

    printf("MERR[%d]: '%s'\n", id, merr);
}

static void print_cseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_CSEQ, id)) {
        printf("unable to read CSEQ #%d\n", id);
        return;
    }
    printf("CSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_gseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_GSEQ, id)) {
        printf("unable to read GSEQ #%d\n", id);
        return;
    }
    printf("GSEQ #%d: length is %d, ", id, chunk.length);

    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_pseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_PSEQ, id)) {
        printf("unable to read PSEQ #%d\n", id);
        return;
    }
    printf("PSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_lseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_LSEQ, id)) {
        printf("unable to read LSEQ #%d\n", id);
        return;
    }
    printf("LSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_bvoc(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;
    uint8_t *data = NULL;

    if (gff_find_chunk_header(gff, &chunk, GFF_BVOC, id)) {
        printf("unable to read BVOC #%d\n", id);
        return;
    }
    printf("BVOC #%d: length is %d, \n", id, chunk.length);

    data = malloc(chunk.length);
    if (gff_read_chunk(gff, &chunk, data, chunk.length) != chunk.length) {
        printf("Can't read BVOC #%d\n", id);
        return;
    }
    free(data);
}

static void print_accl(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;
    uint8_t *data = NULL;
    gff_accl_t *accl;

    if (gff_find_chunk_header(gff, &chunk, GFF_ACCL, id)) {
        printf("unable to read ACCL #%d\n", id);
        return;
    }
    printf("ACCL #%d: length is %d\n ", id, chunk.length);

    data = malloc(chunk.length);
    if (gff_read_chunk(gff, &chunk, data, chunk.length) != chunk.length) {
        printf("Can't read ACCL #%d\n", id);
        return;
    }

    accl = (gff_accl_t*)data;
    for (int i = 0; i < accl->count; i++) {
        printf("    %d: flags: 0x%x event: %d user_id: %d\n", i, accl->entries[i].flags, accl->entries[i].event, accl->entries[i].user_id);
    }

    free(data);
}

static void print_ebox(gff_file_t *gff, unsigned int id) {
    gff_ebox_t ebox;
    //gff_frame_t frame;

    if (gff_read_ebox(gff, id, &ebox)) {
        printf("unable to read EBOX #%d\n", id);
        return;
    }

    printf("EBOX #%d:\n", id);
    printf("    header = {type: %d, len: %d, id: %d}\n", ebox.rh.type, ebox.rh.len, ebox.rh.id);
    printf("    max_lines: %d, ", ebox.max_lines);
    printf("styles: %d, ", ebox.styles);
    printf("runs: %d, ", ebox.runs);
    printf("size: %d\n", ebox.size);
    printf("    user_id: %d\n", ebox.user_id);
    printf("    bounds: { (%d, %d) -> (%d, %d) }\n",
            ebox.frame.bounds.xmin,
            ebox.frame.bounds.ymin,
            ebox.frame.bounds.xmax,
            ebox.frame.bounds.xmax);
    printf("    initbounds: { (%d, %d) -> (%d, %d) }\n",
            ebox.frame.initbounds.xmin,
            ebox.frame.initbounds.ymin,
            ebox.frame.initbounds.xmax,
            ebox.frame.initbounds.xmax);
    printf("    zonebounds: { (%d, %d) -> (%d, %d) }\n",
            ebox.frame.zonebounds.xmin,
            ebox.frame.zonebounds.ymin,
            ebox.frame.zonebounds.xmax,
            ebox.frame.zonebounds.xmax);
    printf("    border_bmp: %d\n", ebox.frame.border_bmp);
    printf("    background_bmp: %d\n", ebox.frame.background_bmp);
}

static void print_frame(gff_file_t *gff, unsigned int id) {
    gff_full_frame_t frame;

    if (gff_read_frame(gff, id, &frame)) {
        printf("unable to read Frame #%d\n", id);
        return;
    }

    printf("APFM #%d:\n", id);
    printf("    header = {type: %d, len: %d, id: %d}\n", frame.rh.type, frame.rh.len, frame.rh.id);
    print_gff_frame(&frame.frame);
    printf("    event_filter: %d\n", frame.event_filter);
    printf("    snap: { mode: %d, (%x, %x), w: %d, h: %d }\n",
            frame.snap_mode,
            frame.snap_x,
            frame.snap_y,
            frame.snap_width,
            frame.snap_height);
}

static void print_window(gff_file_t *gff, unsigned int id) {
    gff_window_t *win = NULL;
    uint8_t      *buf = NULL;

    gff_read_window(gff, id, &win);
    printf("WINDOW #%d:\n", id);
    printf("    header = {type: %d, len: %d, id: %d}\n", win->rh.type, win->rh.len, win->rh.id);
    /*
    printf("    region:\n");
    printf("        depth: %d\n", win->region.depth);
    printf("        bounds: { (%d, %d) -> (%d, %d)\n",
            win->region.bounds.xmin,
            win->region.bounds.ymin,
            win->region.bounds.xmax,
            win->region.bounds.ymax);
    for (int i = 0; i < 16; i++) {
        printf("        region[%d]: { (%d, %d) -> (%d, %d)\n", i,
                win->region.regions[i].xmin,
                win->region.regions[i].ymin,
                win->region.regions[i].xmax,
                win->region.regions[i].ymax);
    }
    */
    printf("    pos: (%d, %d)\n", win->x, win->y);
    printf("    flags: %d\n", win->flags);
    print_gff_frame(&win->frame);
    printf("    offset: (%d, %d)\n", win->offsetx, win->offsety);
    printf("    item_count: %d\n", win->itemCount);

    buf = (uint8_t*)win;
    for (int i = 0; i < win->itemCount; i++) {
        gff_gui_item_t *item = (gff_gui_item_t*)(buf + sizeof(gff_window_t) + 12 + i *(sizeof(gff_gui_item_t)));
        switch (item->type) {
            case GFF_ACCL:
                print_accl(gff, item->id);
                break;
            case GFF_APFM:
                print_frame(gff, item->id);
                break;
            case GFF_BUTN:
                print_button(gff, item->id);
                break;
            case GFF_EBOX:
                print_ebox(gff, item->id);
                break;
            default:
                printf("UNKNOWN TYPE IN WINDOW: %d\n", item->type);
        }
    }

    free(win);
    printf("WINDOW #%d END\n", id);
}

static void print_icon(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_ICON, id);


    printf("ICON #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_ICON, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_port(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_PORT, id);


    printf("PORT #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_PORT, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_wall(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_WALL, id);


    printf("WALL #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_WALL, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_bmp(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_BMP, id);

    printf("BMP #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_BMP, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_gff_entry(gff_file_t *gff, gff_chunk_entry_t *entry) {
    uint32_t len;
    unsigned int *ids;
    gff_chunk_header_t chunk;
    void (*print_func)(gff_file_t *gff, unsigned int id);

    switch (entry->chunk_type) {
        case GFF_WIND: print_func = print_window; break;
        case GFF_BUTN: print_func = print_button; break;
        case GFF_GFFI: print_func = print_gffi; break;
        case GFF_TEXT: print_func = print_text; break;
        case GFF_SPIN: print_func = print_spin; break;
        case GFF_RDAT: print_func = print_rdat; break;
        case GFF_MONR: print_func = print_monr; break;
        case GFF_FONT: print_func = print_font; break;
        case GFF_MERR: print_func = print_merr; break;
        case GFF_CSEQ: print_func = print_cseq; break;
        case GFF_GSEQ: print_func = print_gseq; break;
        case GFF_PSEQ: print_func = print_pseq; break;
        case GFF_LSEQ: print_func = print_lseq; break;
        case GFF_BVOC: print_func = print_bvoc; break;
        case GFF_ACCL: print_func = print_accl; break;
        case GFF_EBOX: print_func = print_ebox; break;
        case GFF_APFM: print_func = print_frame; break;
        case GFF_ICON: print_func = print_icon; break;
        case GFF_BMP: print_func = print_bmp; break;
        case GFF_NAME: print_func = print_name; break;
        case GFF_IT1R: print_func = print_it1r; break;
        case GFF_PORT: print_func = print_port; break;
        case GFF_WALL: print_func = print_wall; break;
        default:
            fprintf(stderr, "printer not written for '%c%c%c%c'\n",
                entry->chunk_type,
                entry->chunk_type >> 8,
                entry->chunk_type >> 16,
                entry->chunk_type >> 24
                );
            exit(1);
            break;
    }

    ids = gff_get_id_list(gff, entry->chunk_type, &len);

    for (int i = 0; i < len; i++) {

        if (gff_find_chunk_header(gff, &chunk, entry->chunk_type, ids[i]) ) {
            fprintf(stderr, "Unable to read chunk %d: id = %d\n", i, ids[i]);
            exit(1);
        }

        print_func(gff, ids[i]);
    }

    free(ids);
}

extern void gffmod_print_entry(gff_file_t *gff, const char *name) {
    int32_t index = -1;

    // See if it is a number.
    if (name[0] == '0') {
        index = 0;
    } else {
        index = atoi(name);
        if (index == 0) { index = -1; }
    }

    // If not a number convert from type to index
    for (int i = 0; index == -1 && i < gff->num_types; i++) {
        gff_chunk_entry_t *entry = gff->chunks[i];
        if (!strncmp((char*)&entry->chunk_type, name, 4)) {
            index = i;
        }
    }

    if (index <= -1) {
        fprintf(stderr, "Unable to find entry.\n");
        exit(1);
    }

    if (index >= gff->num_types) {
        fprintf(stderr, "Entry '%d' out of range (0 - %d).\n", index, gff->num_types - 1);
        exit(1);
    }

    print_gff_entry(gff, gff->chunks[index]);
}

