#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/gui.h>
#include <gff/manager.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *input_file = NULL;
static char *entry_name = NULL;
static char print_toc = 0;
static char *dump = NULL;
static char *rdff_entry = NULL;
static char *img_dir = NULL;
static char *xmi_dir = NULL;
static char *ds1_path = NULL;
static uint8_t verbose = 0;

extern int gffmod_write_image(const char *base_path, gff_file_t *gff, const int type_id, const int res_id);
extern int gffmod_write_xmis(const char *base_path, gff_file_t *gff, const int type_id, const int res_id);
extern void gffmod_print_entry(gff_file_t *gff, const char *name);
extern int gffmod_print_rdff(gff_manager_t *man, gff_file_t *gff, const char *rdff_entry);

gff_manager_t *man;

static void print_help(char *name) {
    printf("usage: %s <options>\n", name);
    printf("       -i <input gff file> : the input file\n");
    printf("       -t <num: optional>  : print the table of contents of gff or entry\n");
    printf("       -w <entry num>      : print the window information\n");
    printf("       -d <dir>            : dump the raw data of each entry of the GFF into the directory <dir>\n");
    printf("       -I <dir>            : dump all possible images (graphics) into <dir>\n");
    printf("       -x <dir>            : dump all possible bvoc AND xmi (midi music with extensions) into <dir>\n");
    printf("       -r <rdff entry>     : display full contents of rdff entry\n");
    printf("       -1 <dir>            : load all DarkSun: Shattered Lands from <dir>\n");
    printf("       -v                  : verbose output\n");
}

static void print_table_of_contents(gff_file_t *gff) {
    printf("There are %d types\n", gff->num_types);

    for (int i = 0; i < gff->num_types; i++) {
        gff_chunk_entry_t *entry = gff->chunks[i];
        printf("Entry %2d: '%c%c%c%c' %s\n", i, 
                entry->chunk_type,
                entry->chunk_type >> 8,
                entry->chunk_type >> 16,
                entry->chunk_type >> 24,
                gff_type_to_str(entry->chunk_type)
              );
    }
}

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


static void dump_entries(gff_file_t *gff, const char *path) {
    unsigned int *ids = NULL;
    uint32_t len;
    char name[256];
    uint8_t *buf;
    size_t buflen;

    // If not a number convert from type to index
    for (int i = 0; i < gff->num_types; i++) {
        gff_chunk_entry_t *entry = gff->chunks[i];
        gff_load_id_list(gff, entry->chunk_type, &ids, &len);
        for (int j = 0; j < len; j++) {
            buflen = gff_load_raw(gff, entry->chunk_type, ids[j], &buf);
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
    while ((c = getopt (argc, argv, "i:ht::p:d:I:vx:1:r:")) != -1) {
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
            case 'x':
                xmi_dir = optarg;
                break;
            case 'd':
                dump = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'r':
                rdff_entry = optarg;
                break;
            case '1':
                ds1_path = optarg;
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
        //printf("type: %d, len: %d\n", type, len);
        gff_get_resource_ids(f, type, res_ids, &pos);
        for (int j = 0; j < len; j++) {
            gffmod_write_image(base_path, f, type, res_ids[j]);
        }
    }   
}

static void extract_all_bvocs(gff_file_t *gff, const char *base_path) {
    uint32_t res_ids[1024];
    char bvoc_path[256];
    gff_chunk_header_t chunk;
    uint32_t len, pos;
    uint8_t *data = NULL;

    printf("Start write of all bvocs.\n");

    for (int i = 0; i < gff_get_number_of_types(gff); i++) {
        int type;
        gff_get_resource_length(gff, type = gff_get_type_id(gff, i), &len);
        //printf("type: %d, len: %d\n", type, len);
        gff_get_resource_ids(gff, type, res_ids, &pos);
        if (type != GFF_BVOC) { continue; }
        for (int j = 0; j < len; j++) {
        //snprintf(filename, 1<<10, "%s/%d-%s-res%d-frame%d.bmp", base_path, gff->id, type, res_id, cframe);
            snprintf(bvoc_path, 255, "%s%s-BVOC-%d.bvoc", base_path, gff->filename, res_ids[j]);
            if (gff_find_chunk_header(gff, &chunk, GFF_BVOC, res_ids[j])) {
                printf("unable to read BVOC #%d\n", res_ids[j]);
                return;
            }
            data = malloc(chunk.length);
            if (gff_read_chunk(gff, &chunk, data, chunk.length) != chunk.length) {
                printf("Can't read BVOC #%d\n", res_ids[j]);
                return;
            }
            printf("BVOC: %s\n", bvoc_path);
            FILE *f = fopen(bvoc_path, "w+");
            fwrite(data, 1, chunk.length, f);
            fclose(f);
            free(data);
            data = NULL;
        }
    }

    printf("End write of all bvocs.\n");
}

void extract_all_xmis(gff_file_t *f, const char *base_path) {
    uint32_t res_ids[1024];
    uint32_t len, pos;
    char pseq[128];
    char cseq[128];
    char gseq[128];
    char lseq[128];

    printf("Start write of all xmis/midis.\n");

    snprintf(pseq, 127, "%s/pseq", base_path);
    snprintf(cseq, 127, "%s/cseq", base_path);
    snprintf(gseq, 127, "%s/gseq", base_path);
    snprintf(lseq, 127, "%s/lseq", base_path);
    for (int i = 0; i < gff_get_number_of_types(f); i++) {
        int type;
        gff_get_resource_length(f, type = gff_get_type_id(f, i), &len);
        //printf("type: %d, len: %d\n", type, len);
        gff_get_resource_ids(f, type, res_ids, &pos);
        for (int j = 0; j < len; j++) {
            switch (type) {
                case GFF_CSEQ: gffmod_write_xmis(cseq, f, type, res_ids[j]); break;
                case GFF_GSEQ: gffmod_write_xmis(gseq, f, type, res_ids[j]); break;
                case GFF_PSEQ: gffmod_write_xmis(pseq, f, type, res_ids[j]); break;
                case GFF_LSEQ: gffmod_write_xmis(lseq, f, type, res_ids[j]); break;
            }
        }
    }
    printf("End write of all xmis/midis.\n");
}

static void do_mods() {
    man = gff_manager_create();
    gff_file_t *gff = gff_allocate();

    if (!input_file) {
        printf("You must FIRST specify an input file (-i <gff file>)\n");
        exit(1);
    }

    gff_init(gff);

    gff_manager_load_ds1(man, ds1_path);

    if (gff_open(gff, input_file)) {
        fprintf(stderr, "Unable to open '%s'.\n", input_file);
        exit(1);
    }

    if (print_toc) {
        if (entry_name) {
            gffmod_print_entry(gff, entry_name);
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

    if (xmi_dir) {
        extract_all_xmis(gff, xmi_dir);
        extract_all_bvocs(gff, xmi_dir);
    }

    if (rdff_entry) {
        gffmod_print_rdff(man, gff, rdff_entry);
    }

    gff_free(gff);
    gff_manager_free(man);
}

int main(int argc, char *argv[]) {
    parse_args(argc, argv);

    do_mods();
}

