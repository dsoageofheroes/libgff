#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/gui.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *input_file = NULL;
static char *entry_name = NULL;
static char print_toc = 0;
static char *dump = NULL;
static char *img_dir = NULL;
static uint8_t verbose = 0;

extern int gffmod_write_image(const char *base_path, gff_file_t *gff, const int type_id, const int res_id);

static void print_help(char *name) {
    printf("usage: %s <options>\n", name);
    printf("       -i <input gff file> : the input file\n");
    printf("       -t <num: optional>  : print the table of contents of gff or entry\n");
    printf("       -w <entry num>      : print the window information\n");
    printf("       -d <dir>            : dump the raw data ofeach entry of the GFF into the directory <dir>\n");
    printf("       -I <dir>            : dump all possible images (graphics) into <dir>\n");
    printf("       -v                  : verbose output\n");
}

static void print_table_of_contents(gff_file_t *gff) {
    printf("There are %d types\n", gff->num_types);

    for (int i = 0; i < gff->num_types; i++) {
        gff_chunk_entry_t *entry = gff->chunks[i];
        printf("Entry %2d: '%c%c%c%c'\n", i, 
                entry->chunk_type,
                entry->chunk_type >> 8,
                entry->chunk_type >> 16,
                entry->chunk_type >> 24
              );
    }
}

typedef struct gff_accl_entry_s {
    uint8_t  flags;
    uint16_t event;
    uint16_t user_id;
} gff_accl_entry_t;

typedef struct gff_accl_s {
    gff_resource_header_t rh;
    uint16_t              count;
    gff_accl_entry_t      entries[];
} gff_accl_t;

/*
static int print_accl(gff_file_t *gff, const int res_id) {
    gff_chunk_header_t chunk;
    gff_accl_t        *accl;
    int                amt;
    char               buf[1024];

    if (gff_find_chunk_header(gff, &chunk, GFF_ACCL, res_id)) {
        goto no_header;
    }

    //printf("chunk.length = %d, %d\n", chunk.length, sizeof(gff_accl_t));
    //if ((amt = gff_read_chunk(man->ds1.resource, &chunk, &accl, sizeof(gff_accl_t))) < sizeof(gff_accl_t)) {
    if ((amt = gff_read_chunk(gff, &chunk, buf, 1024)) < sizeof(gff_accl_t)) {
        //printf("amt = %d\n", amt);
        goto read_header_error;
    }

    accl = (gff_accl_t*)buf;
    printf("ACCL: id: %d, type: %d, len: %d, count:%d\n", accl->rh.id, accl->rh.type, accl->rh.len, accl->count);
    for (int i = 0; i < accl->count; i++) {
        printf("    %d: flags: 0x%x event:%d id: %d\n", i, accl->entries[i].flags, accl->entries[i].event, accl->entries[i].user_id);
    }
    //printf("'%c%c%c%c'\n", buf[0], buf[1], buf[2], buf[3]);

    return EXIT_FAILURE;
read_header_error:
no_header:
    printf("ACCL: error!\n");
    return EXIT_FAILURE;

}
    */

/*
static int print_frame(gff_file_t *gff, const int res_id) {
    char buf[4096];
    gff_chunk_header_t chunk;
    gff_app_frame_t *gff_frame;
    int amt;
   
    if (gff_find_chunk_header(gff, &chunk, GFF_APFM, res_id)) {
        goto no_header;
    }

    if ((amt = gff_read_chunk(gff, &chunk, buf, 4096)) <= 0) {
        goto read_error;
    }

    gff_frame = (gff_app_frame_t*)buf;
    printf(" id: %d, w: %d, h: %d, bmp: %d/%d, flags = 0x%x, type = 0x%x "
            " snapMode = %d, event = 0x%x"
            " (%d, %d -> %d, %d)\n",
            gff_frame->rh.id,
            gff_frame->frame.width,
            gff_frame->frame.height,
            gff_frame->frame.background_bmp,
            gff_frame->frame.border_bmp,
            gff_frame->frame.flags,
            gff_frame->frame.type,
            gff_frame->snapMode,
            gff_frame->event_filter,
            gff_frame->frame.zonebounds.xmin, gff_frame->frame.zonebounds.ymin,
            gff_frame->frame.initbounds.xmin, gff_frame->frame.initbounds.ymin
            //gff_frame->frame.bounds.xmin, gff_frame->frame.bounds.ymin,
            //gff_frame->frame.zonebounds.xmax, gff_frame->frame.zonebounds.ymax
            );
    return EXIT_SUCCESS;
read_error:
no_header:
    return EXIT_FAILURE;
}
*/

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

static void print_gff_entry(gff_file_t *gff, gff_chunk_entry_t *entry) {
    uint32_t len;
    unsigned int *ids;
    gff_chunk_header_t chunk;
    void (*print_func)(gff_file_t *gff, unsigned int id);

    switch (entry->chunk_type) {
        case GFF_WIND:
            print_func = print_window;
            break;
        case GFF_BUTN:
            print_func = print_button;
            break;
        case GFF_GFFI: print_func = print_gffi; break;
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

static void print_entry(gff_file_t *gff, const char *name) {
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

static void dump_entries(gff_file_t *gff, const char *path) {
    unsigned int *ids = NULL;
    uint32_t len;
    char name[256];
    char *buf;
    size_t buflen;

    // If not a number convert from type to index
    for (int i = 0; i < gff->num_types; i++) {
        gff_chunk_entry_t *entry = gff->chunks[i];
        ids = gff_get_id_list(gff, entry->chunk_type, &len);
        for (int j = 0; j < len; j++) {
            buflen = gff_read_raw_allocate(gff, entry->chunk_type, ids[j], &buf);
            snprintf(name, 255, "%s/%c%c%c%c%d", path,
                entry->chunk_type,
                entry->chunk_type >> 8,
                entry->chunk_type >> 16,
                entry->chunk_type >> 24,
                ids[j]
                );
            FILE *f = fopen(name, "wb+");
            fwrite(buf, 1, buflen, f);
            fclose(f);
        }
    }
}

static void parse_args(int argc, char *argv[]) {
    char c;
    while ((c = getopt (argc, argv, "i:ht::p:d:I:v")) != -1) {
        switch(c) {
            case 'i':
                input_file = optarg;
                break;
            case 'h':
                print_help(argv[0]);
                break;
            case 't':
                print_toc = 1;
                if (optarg == NULL && optind < argc && argv[optind][0] != '-') {
                    entry_name = optarg = argv[optind++];
                }
                //if (entry_name) { printf("->%s\n", entry_name); }
                break;
            case 'I':
                img_dir = optarg;
                break;
            case 'd':
                dump = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
        }
    }
}

void extract_all_images(gff_file_t *f, const char *base_path) {
    uint32_t res_ids[1024];
    uint32_t len, pos;
    printf("START EXPORT ALL IMAGES\n");
    for (int i = 0; i < gff_get_number_of_types(f); i++) {
        int type;
        gff_get_resource_length(f, type = gff_get_type_id(f, i), &len);
        printf("type: %d, len: %d\n", type, len);
        gff_get_resource_ids(f, type, res_ids, &pos);
        for (int j = 0; j < len; j++) {
            gffmod_write_image(base_path, f, type, res_ids[j]);
        }
    }   
}

static void do_mods() {
    gff_file_t *gff = gff_allocate();

    if (!input_file) {
        printf("You must FIRST specify an input file (-i <gff file>)\n");
        exit(1);
    }

    gff_init(gff);

    if (gff_open(gff, input_file)) {
        fprintf(stderr, "Unable to open '%s'.\n", input_file);
        exit(1);
    }

    if (print_toc) {
        if (entry_name) {
            print_entry(gff, entry_name);
        } else {
            print_table_of_contents(gff);
        }
    }

    if (dump) {
        dump_entries(gff, dump);
    }

    if (img_dir) {
        extract_all_images(gff, img_dir);
    }

    gff_free(gff);
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    do_mods();
}

