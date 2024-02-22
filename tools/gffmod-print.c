#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/gui.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_button(gff_file_t *gff, unsigned int id) {
    gff_button_t button;

    gff_read_button(gff, id, &button);
    printf("button: %d size (%d, %d) icon: %d", id, 
            button.frame.width, button.frame.height,
            button.icon_id
            );
}

static void print_window(gff_file_t *gff, unsigned int id) {
    gff_window_t *win = NULL;
    uint8_t      *buf = NULL;

    gff_read_window(gff, id, &win);
    printf("window: %d, pos (%d, %d) size (%d, %d) with %d items.\n", id, 
            win->x, win->y,
            win->frame.width, win->frame.height,
            win->itemCount
            );

    printf("background bmp: %d boder bmp:%d\n", win->frame.background_bmp, win->frame.border_bmp);
    /*
    printf("%d, %d, %d, %d\n",
            win->region.bounds.xmin,
            win->region.bounds.xmax,
            win->region.bounds.ymin,
            win->region.bounds.ymax);
    for (int i = 0; i < 16; i++) {
    printf("%d, %d, %d, %d\n",
            win->region.regions[i].xmin,
            win->region.regions[i].xmax,
            win->region.regions[i].ymin,
            win->region.regions[i].ymax);
    }
    */
    buf = (uint8_t*)win;
    for (int i = 0; i < win->itemCount; i++) {
        gff_gui_item_t *item = (gff_gui_item_t*)(buf + sizeof(gff_window_t) + 12 + i *(sizeof(gff_gui_item_t)));
        switch (item->type) {
            case GFF_ACCL:
                printf("%d: ACCL %d\n", i, item->id);
                //print_accl(gff, item->id);
                break;
            case GFF_APFM:
                printf("%d: APFM %d\n", i, item->id);
                //print_frame(gff, item->id);
                break;
            case GFF_BUTN:
                printf("%d: ", i);
                print_button(gff, item->id);
                printf("\n");
                break;
            case GFF_EBOX:
                printf("%d: EBOX %d\n", i, item->id);
                break;
            default:
                printf("UNKNOWN TYPE IN WINDOW: %d\n", item->type);
        }
    }
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

    printf("Text[%d]: '%s'", id, text);
}

static void print_spin(gff_file_t *gff, unsigned int id) {
    char spin[4096];

    if (gff_read_spin(gff, id, spin, 4096)) {
        printf("Unable to read SPIN @ %d\n", id);
    }

    printf("SPIN[%d]: '%s'", id, spin);
}

static void print_rdat(gff_file_t *gff, unsigned int id) {
    char rdat[4096];

    if (gff_read_spin(gff, id, rdat, 4096)) {
        printf("Unable to read RDAT @ %d\n", id);
    }

    printf("RDAT[%d]: '%s'", id, rdat);
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

    printf("Warning: GFF_MONR may not be accurate.");

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
    printf("Need to complete font...");

    free(font);
}

static void print_merr(gff_file_t *gff, unsigned int id) {
    char merr[4096];

    if (gff_read_merr(gff, id, merr, 4096)) {
        printf("Unable to read MERR @ %d\n", id);
    }

    printf("MERR[%d]: '%s'", id, merr);
}

static void print_cseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_CSEQ, id)) {
        printf("unable to read CSEQ #%d\n", id);
        return;
    }
    printf("CSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.");
}

static void print_gseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_GSEQ, id)) {
        printf("unable to read GSEQ #%d\n", id);
        return;
    }
    printf("GSEQ #%d: length is %d, ", id, chunk.length);

    printf("Use `-x <dir>` to extract all xmis to a directory.");
}

static void print_pseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_PSEQ, id)) {
        printf("unable to read PSEQ #%d\n", id);
        return;
    }
    printf("PSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.");
}

static void print_lseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_LSEQ, id)) {
        printf("unable to read LSEQ #%d\n", id);
        return;
    }
    printf("LSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.");
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
        printf("\n");
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

