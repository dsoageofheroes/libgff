#include "gff.h"
#include "gfftypes.h"
#include "gpl.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#define BUF_SIZE (1024)
gff_file_t open_files[NUM_FILES];
static int master_gff;

static int write_toc(const int gff_idx);
static int get_next_idx(const char *name);
static int gff_close_file(gff_file_t *gff);

int gff_get_master() {
    return master_gff;
}

static char* strtolwr(char *str) {

    for (char *c = str; *c; c++) {
        *(c) = tolower(*c);
    }

    return str;
}

// Precondition, name is in lowercase!
int gff_find_index(const char *name) {
    if (!name) { return -1; }

    for (int i = 0; i < NUM_FILES; i++) {
        //printf("comparing: '%s' ?= '%s'\n", name, open_files[i].filename);
        if (open_files[i].filename 
            && strcmp(name, open_files[i].filename) == 0) {
            return i;
        }
    }

    return -1;
}

// Precondition, name is in lowercase!
int get_next_idx(const char *name) {
    int i;

    if (gff_find_index(name) > -1) {
        fprintf(stderr, "'%s' is already loaded!\n", name);
        return -1; // We already have that file!
    }

    if (strcmp(name, "resource.gff") == 0) { return RESOURCE_GFF_INDEX; }
    if (strcmp(name, "segobjex.gff") == 0) { return OBJEX_GFF_INDEX; }
    if (strcmp(name, "objex.gff") == 0) { return OBJEX_GFF_INDEX; }
    if (strcmp(name, "gpldata.gff") == 0) { return DSLDATA_GFF_INDEX; }
    if (strcmp(name, "charsave.gff") == 0) { return CHARSAVE_GFF_INDEX; }
    if (strcmp(name, "darksave.gff") == 0) { return DARKSAVE_GFF_INDEX; }
    if (strcmp(name, "cine.gff") == 0) { return CINE_GFF_INDEX; }
    if (strcmp(name, "darkrun.gff") == 0) { return DARKRUN_GFF_INDEX; }
    if (strcmp(name, "resflop.gff") == 0) { return RESFLOP_GFF_INDEX; }

    for (i = REST_GFF_INDEX; i < NUM_FILES; i++) {
        if (!open_files[i].file) {
            return i;
        }
    }

    fprintf(stderr, "Number of GFF slots are full!\n");
    return -1;
}

/*
void gff_init() {
    memset(open_files, 0, sizeof(gff_file_t) * NUM_FILES);

    master_gff = -1;
    gff_image_init();
}
*/

extern int gff_init(gff_file_t *f) {
    if (!f) { return EXIT_FAILURE; }

    memset(f, 0, sizeof(gff_file_t));

    return EXIT_SUCCESS;
}

extern gff_file_t* gff_allocate() {
    gff_file_t *ret = NULL;

    if (!(ret = malloc(sizeof(gff_file_t)))) {
        goto memory_error;
    }

    if (gff_init(ret)) {
        goto init_error;
    }

    return ret;
init_error:
    free(ret);
memory_error:
    return NULL;
}

extern int gff_free(gff_file_t *f) {
    if (!f) { goto bad_arg; }

    gff_palettes_free(f);

    //TODO: CHECK 
    if (gff_close_file(f)) {
        goto close_failure;
    }

    if (gff_init(f)) {
        goto init_error;
    }

    free(f);

    return EXIT_SUCCESS;

init_error:
close_failure:
bad_arg:
    return EXIT_FAILURE;
}

static int is_gff_filename(const char *str) {
    int slen = strlen (str);

    return slen > 4 
        && (str[slen-1] == 'f' || str[slen-1] == 'F')
        && (str[slen-2] == 'f' || str[slen-2] == 'F')
        && (str[slen-3] == 'g' || str[slen-3] == 'G');
}

void gff_load_directory(const char *path) {
    DIR *dir;
    struct dirent *ent;
    char buf[BUF_SIZE];
    gff_file_t f;

    debug("Loading GFFs from: %s\n", path);
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (is_gff_filename(ent->d_name)) {
                snprintf(buf, BUF_SIZE, "%s/%s", path, ent->d_name);
                //gff_open(buf);
                gff_open(&f, buf);
            }
        }
        closedir (dir);
    } else {
        fprintf(stderr, "Unable to open directory: '%s'\n", path);
    }
}

/*
const enum game_type_e gff_get_game_type() {
    if (open_files[RESOURCE_GFF_INDEX].filename && open_files[RESFLOP_GFF_INDEX].filename) {
        return DARKSUN_2;
    }

    if (open_files[RESOURCE_GFF_INDEX].filename && !open_files[RESFLOP_GFF_INDEX].filename) {
        return DARKSUN_1;
    }

    if (!open_files[RESOURCE_GFF_INDEX].filename && open_files[RESFLOP_GFF_INDEX].filename) {
        return DARKSUN_ONLINE;
    }

    return DARKSUN_UNKNOWN;
}
*/

const char** gff_list(size_t *len) {
    *len = 0;
    const char **ret = NULL;
    int pos = 0;

    for (int i = 0; i < NUM_FILES; i++) {
        if (open_files[i].filename) { (*len)++; }
    }

    if (*len == 0) { return NULL; }

    ret = malloc(sizeof(char*) * *len);
    for (int i = 0; i < NUM_FILES; i++) {
        if (open_files[i].filename) { 
            ret[pos] = open_files[i].filename;
            pos++;
        }
    }

    return ret;
}

int gff_create(const char *path) {
    int id = get_next_idx(path);
    FILE * file = fopen(path, "wb+");
    gff_file_t *gff = open_files + id;

    gff->file = file;
	gff->header.identity        = GFF_GFFI;
	gff->header.version         = GFFVERSION;
	gff->header.data_location   = 28L;
	gff->header.toc_location    = 28L;
	gff->header.toc_length      = 10L; // types is 2B, toc is 8B
	gff->header.file_flags      =  0L;
	gff->header.data0           =  0L;

    gff->toc.types_offset       =  8L;
    gff->toc.free_list_offset   = 10L;

    gff->num_types              = 0L;
    gff->pals = NULL;

    write_toc(id);

    return id;
}

size_t gff_add_type(const int idx, const int type_id) {
    gff_file_t *gff = open_files + idx;
    
    for (int i = 0; i < gff->num_types; i++) {
        if ((gff->chunks[i]->chunk_type & GFFMAXCHUNKMASK) == type_id) {
            return 0; // Type already exists!
        }
    }

    gff->num_types++;
    gff->chunks = realloc(gff->chunks, sizeof(gff_chunk_entry_t*) * gff->num_types);
    gff_chunk_entry_t *new = malloc(8L); // Only allocate the initial 8 bytes worry about extensions later.
    gff->chunks[gff->num_types - 1] = new;
    new->chunk_type = type_id;
    new->chunk_count = 0;

    gff->header.toc_length += 8L;
    gff->toc.free_list_offset += 8L;

    write_toc(idx);

    return 1;
}

static char* get_filename_from_path(const char *path) {
    int len = strlen(path);
    int i = 0;

    if (len < 3) { return NULL; }
    for (i = len-1; i >= 0 && path[i] != '/' && path[i] != '\\' ; i--) {
    }

    return strdup(path + i + 1);
}

/*
static int is_master_name(const char *name) {
    if (strcmp(name, "resource.gff") == 0) { return 1; }
    if (strcmp(name, "resflop.gff") == 0) { return 1; }

    return 0;
}
*/

typedef struct {
  uint32_t	  chunkType;
  uint32_t 	  chunkCount;
} gff_chunk_list_header_t;

static void gff_read_headers(gff_file_t *gff) {
    fseek(gff->file, 0, SEEK_SET);
    if (fread(&(gff->header), 1, sizeof(gff_file_header_t), gff->file)
            != sizeof(gff_file_header_t)) {
        fatal("Unable to read header!\n");
    }

    fseek(gff->file, gff->header.toc_location, SEEK_SET);
    if (fread(&(gff->toc), 1, sizeof(gff_toc_header_t), gff->file)
            != sizeof(gff_toc_header_t)) {
        fatal("Unable to read Table of Contents!\n");
    }

    fseek(gff->file, gff->header.toc_location + gff->toc.types_offset, SEEK_SET);
    //if (fread(&(gff->types), 1, sizeof(gff_type_header_t), gff->file)
            //!= sizeof(gff_type_header_t)) {
    if (fread(&(gff->num_types), 1, sizeof(uint16_t), gff->file)
            != sizeof(uint16_t)) {
        fatal("Unable to read types header!\n");
    }

    gff->chunks = malloc(sizeof(gff_chunk_entry_t*) * gff->num_types);
    memset(gff->chunks, 0x0, sizeof(gff_chunk_entry_t*) * gff->num_types);

    int i = 0;

    gff_chunk_list_header_t chunk_header;
    fseek(gff->file, gff->header.toc_location + gff->toc.types_offset + 2L, SEEK_SET);
    seg_header_t seg_header;
    for (i = 0; i < gff->num_types; i++) {
        fread(&(chunk_header), 1, sizeof(gff_chunk_list_header_t), gff->file);
        gff_chunk_entry_t *chunk = NULL;

        if (chunk_header.chunkCount & GFFSEGFLAGMASK) {
            fread(&(seg_header), 1, sizeof(seg_header_t), gff->file);

            chunk = malloc(8L + sizeof(gff_seg_t) + seg_header.num_entries * sizeof(gff_seg_entry_t));
            chunk->chunk_type = chunk_header.chunkType;
            chunk->chunk_count = chunk_header.chunkCount;
            chunk->segs.header = seg_header;

            fread(&(chunk->segs.segs), 1, seg_header.num_entries * sizeof(gff_seg_entry_t), gff->file);
            gff->chunks[i] = chunk;
        } else {
            chunk = malloc(8L + sizeof(gff_chunk_header_t) * chunk_header.chunkCount);
            chunk->chunk_type = chunk_header.chunkType;
            chunk->chunk_count = chunk_header.chunkCount;
            fread(&(chunk->chunks), 1, sizeof(gff_chunk_header_t) * chunk_header.chunkCount, gff->file);
            gff->chunks[i] = chunk;
        }

        if (chunk && ((chunk->chunk_type & GFFMAXCHUNKMASK) == GFF_GFFI)) {
            gff->gffi = chunk;
        }
    }
}

int gff_open(gff_file_t *f, const char *pathName) {
    //int idx;
    FILE *file;
    char *filename;

    if (!f) { return EXIT_FAILURE; }

    filename = strtolwr(get_filename_from_path(pathName));

    file = fopen(pathName, "rb+");
    if (!file) {
        f->status = GFF_DNE;
        goto dne;
    }

    fseek(file, 0L, SEEK_SET);

    //printf("Detected file size of '%s': %d\n", filename, len);
    f->num_palettes = 0;
    f->num_objects = -1;
    f->entry_table = NULL;
    f->file = file;

    gff_read_headers(f);

    f->filename = filename;
    f->start_palette_index = gff_get_number_of_palettes();
    gff_create_palettes(f, &(f->num_palettes));
    debug("'%s' loaded as '%s'\n", pathName, f->filename);

    return EXIT_SUCCESS;
dne:
    free(filename);
    return EXIT_FAILURE;
}

extern int gff_get_number_of_types(gff_file_t *f) {
    if (!f || f->file == NULL) { return -1; }

    return f->num_types;
}

extern int gff_get_type_id(gff_file_t *f, int type_index) {
    if (!f || f->file == NULL || type_index < 0 
            || type_index >= f->num_types) { 
        return -1;  // failure.
    }

    return f->chunks[type_index]->chunk_type & GFFMAXCHUNKMASK;
}

//gff_chunk_header_t gff_find_chunk_header(int idx, int type_id, int res_id) {
extern int gff_find_chunk_header(gff_file_t *f, gff_chunk_header_t *chunk, int type_id, int res_id) {
    //gff_file_t *gff = open_files + idx;
    gff_chunk_entry_t *entry = NULL;
    gff_seg_loc_entry_t seg;
    gff_chunk_header_t *gffi_chunk_header = NULL;
    *chunk = (gff_chunk_header_t){0, 0, 0};

    if (!f || !chunk) { return EXIT_FAILURE; }

    for (int i = 0; !entry && i < f->num_types; i++) {
        if (f->chunks && (f->chunks[i]->chunk_type & GFFMAXCHUNKMASK) == type_id) {
            entry = f->chunks[i];
        }
    }
    
    if (!entry) {
        //error("Can't find type %d in gff %d\n", type_id, idx);
        f->status = GFF_NO_TYPE;
        goto entry_error;
    }

    if (entry->chunk_count & GFFSEGFLAGMASK) {
        int32_t chunk_offset = 0;
        gffi_chunk_header = f->gffi->chunks + entry->segs.header.seg_loc_id;
        for (int j = 0; j < entry->segs.header.num_entries; j++) {
            int32_t first_id = entry->segs.segs[j].first_id;
            if (res_id >= first_id && res_id <= (first_id + entry->segs.segs[j].num_chunks)) {
                int offset = 4L + (sizeof(gff_seg_loc_entry_t) * chunk_offset);

                fseek(f->file, gffi_chunk_header->location + offset + (res_id - first_id)*sizeof(gff_seg_loc_entry_t), SEEK_SET);
                fread(&seg, 1, sizeof(gff_seg_loc_entry_t), f->file);

                chunk->id = res_id;
                chunk->location = seg.seg_offset;
                chunk->length = seg.seg_length;

                goto success;
            }
            chunk_offset += entry->segs.segs[j].num_chunks;
        }
    } else {
        for (uint32_t j = 0; j < entry->chunk_count; j++) {
            if (entry->chunks[j].id == res_id) {
                chunk->id = res_id;
                chunk->location = entry->chunks[j].location;
                chunk->length = entry->chunks[j].length;
                goto success;
            }
        }
    }

    goto not_found;
success:
    return EXIT_SUCCESS;
not_found:
entry_error:
    return EXIT_FAILURE;
}

#define TOC_BUF_SIZE (1<<10)

static int write_toc(const int gff_idx) {
    gff_file_t *gff = open_files + gff_idx;
    static char buf[TOC_BUF_SIZE];

    memset(buf, 0x0, TOC_BUF_SIZE);

    fseek(gff->file, 0, SEEK_SET);
    if (fwrite(&(gff->header), 1, sizeof(gff_file_header_t), gff->file)
            != sizeof(gff_file_header_t)) {
        fatal("Unable to write header!\n");
    }

    fseek(gff->file, gff->header.toc_location, SEEK_SET);
    if ((fwrite(&(gff->toc), 1, sizeof(gff_toc_header_t), gff->file))
            != sizeof(gff_toc_header_t)) {
        fatal("Unable to write Table of Contents header!\n");
    }

    fseek(gff->file, gff->header.toc_location + gff->toc.types_offset, SEEK_SET);
    //if ((wamt = fwrite(&(gff->types), 1, sizeof(gff_type_header_t), gff->file))
            //!= sizeof(gff_type_header_t)) {
    if ((fwrite(&(gff->num_types), 1, sizeof(uint16_t), gff->file))
            != sizeof(uint16_t)) {
        fatal("Unable to write types header!\n");
    }

    fseek(gff->file, gff->header.toc_location + gff->toc.types_offset + 2L, SEEK_SET);
    for (int i = 0; i < gff->num_types; i++) {
        gff_chunk_entry_t *chunk = gff->chunks[i];
        fwrite(&(chunk->chunk_type), 1, sizeof(uint32_t), gff->file);
        fwrite(&(chunk->chunk_count), 1, sizeof(uint32_t), gff->file);
        if (chunk->chunk_count & GFFSEGFLAGMASK) {
            error("SEGMENT TOC Writting not implemented.\n");
            exit(1);
            fseek(gff->file, sizeof(seg_header_t), SEEK_SET);

            //fread(&(chunk->segs.segs), 1, seg_header.num_entries * sizeof(gff_seg_entry_t), gff->file);
            fseek(gff->file, chunk->segs.header.num_entries * sizeof(gff_seg_entry_t), SEEK_CUR);
        } else {
            //printf("%d: write %d chunks\n", i, chunk->chunk_count);
            fwrite(&(chunk->chunks), 1, sizeof(gff_chunk_header_t) * chunk->chunk_count, gff->file);
        }
    }

    //printf("toc_length = %d, free_list-offset = %d\n", gff->header.toc_length, gff->toc.free_list_offset);
    size_t bytes_to_write = gff->header.toc_length - gff->toc.free_list_offset;
    if (bytes_to_write > TOC_BUF_SIZE) {
        error ("overflowed buffer, make a loop!");
        return 0;
    }
    fwrite(buf, 1, bytes_to_write, gff->file);

    return 1;
}

size_t gff_add_chunk(const int idx, const int type_id, int res_id, char *buf, const size_t len) {
    gff_chunk_entry_t *entry = NULL;
    gff_file_t *gff = open_files + idx;
    int chunk_idx;

    for (chunk_idx = 0; !entry && chunk_idx < gff->num_types; chunk_idx++) {
        if ((gff->chunks[chunk_idx]->chunk_type & GFFMAXCHUNKMASK) == type_id) {
            entry = gff->chunks[chunk_idx];
            break;
        }
    }

    if (!entry) {
        error("type not found.\n");
        return 0;
    } // Didn't find...

    if (entry->chunk_count & GFFSEGFLAGMASK) {
        error("can't add to a segment.\n");
        return 0;
    } 

    // It looks like it is best to just increase the amount (reuse is usually determined internally to the resource.)
    // Find out if TOC is at the end of the file.
    fseek(gff->file, 0L, SEEK_END);
    size_t size = ftell(gff->file);
    size_t write_pos = (size == (gff->header.toc_length + gff->header.toc_location)) ? gff->header.toc_location : size;
    for (size_t i = 0; i < entry->chunk_count; i++) {
        if (entry->chunks[i].id == res_id) {
            error("id %d for that type already exists!\n", res_id);
            return 0;
        }
    }
    entry->chunk_count++;
    entry = realloc(entry, 8L + entry->chunk_count * sizeof(gff_chunk_header_t));
    entry->chunks[entry->chunk_count - 1].id = res_id;
    entry->chunks[entry->chunk_count - 1].location = gff->header.toc_location;
    entry->chunks[entry->chunk_count - 1].length = len;

    write_pos += len;

    gff->header.toc_location = write_pos;
    gff->header.toc_length += sizeof(gff_chunk_header_t);
    gff->toc.free_list_offset += sizeof(gff_chunk_header_t);
    gff->chunks[chunk_idx] = entry; // in case a new memory location was allocated.

    // Now write the new chunk to the end of the file.
    fseek(gff->file, entry->chunks[entry->chunk_count - 1].location, SEEK_SET);
    fwrite(buf, 1, len, gff->file);

    // Write out the TOC to the end of the file (you will need to re-alloc the TOC header.)
    return write_toc(idx);
}

//size_t gff_read_chunk_length(int idx, int type_id, int res_id, void *read_buf, const size_t len) {
size_t gff_read_chunk_length(gff_file_t *f, int type_id, int res_id, void *read_buf, const size_t len) {
    gff_chunk_header_t chunk;
    gff_find_chunk_header(f, &chunk, type_id, res_id);
    return chunk.length;
}

extern size_t gff_read_chunk(gff_file_t *f, gff_chunk_header_t *chunk, void *read_buf, const size_t len) {
    if (!f || !f->file || !chunk || chunk->length > len) {
        return 0;
    }

    fseek(f->file, chunk->location, SEEK_SET);
    return fread(read_buf, 1, chunk->length, f->file);
}

//extern size_t gff_read_chunk_piece(int idx, gff_chunk_header_t *chunk, void *read_buf, const size_t len) {
extern size_t gff_read_chunk_piece(gff_file_t *f, gff_chunk_header_t *chunk, void *read_buf, const size_t len) {
    size_t min = chunk->length > len ? len : chunk->length;
    fseek(f->file, chunk->location, SEEK_SET);
    return fread(read_buf, 1, min, f->file);
}

extern size_t gff_read_raw_bytes(gff_file_t *f, int type_id, int res_id, void *read_buf, const size_t len) {
    gff_chunk_header_t chunk;
    gff_find_chunk_header(f, &chunk, type_id, res_id);
    return gff_read_chunk(f, &chunk, read_buf, len);
}

extern size_t gff_write_chunk(gff_file_t *f, const gff_chunk_header_t chunk, const char *path) {
    fseek(f->file, chunk.location, SEEK_SET);
    return fwrite(path, 1, chunk.length, f->file);
}

extern int gff_write_raw_bytes(gff_file_t *f, int type_id, int res_id, const char *path) {
    int amt = 0;
    char *data;
    FILE *file;

    gff_chunk_header_t chunk;
    gff_find_chunk_header(f, &chunk, type_id, res_id);
    data = malloc(chunk.length);

    if (!gff_read_chunk(f, &chunk, data, chunk.length) ) { return 0; }

    file = fopen(path, "w+");
    if (!file) { return 0; }

    amt = fwrite(data, 1, chunk.length, file);

    fclose (file);
    return amt;
}

/*
 * Return the number of ids within a certain file (idx) and type (type_id).
 * Is this function needed?
 */
extern int gff_get_resource_length(gff_file_t *f, int type_id, uint32_t *len) {
    gff_seg_header_t  *seg_header;
    *len = 0;

    if (!f || !len || !f->file) { return EXIT_FAILURE; }

    //for (int i = 0; i < open_files[idx].num_types; i++) {
    for (int i = 0; i < f->num_types; i++) {
        if ((f->chunks[i]->chunk_type & GFFMAXCHUNKMASK) != type_id) { continue; }
        if ((f->chunks[i]->chunk_count) & GFFSEGFLAGMASK) {
            seg_header = (gff_seg_header_t*)&(f->chunks[i]->chunks[0]);
            for (int j = 0; j < seg_header->segRef.num_entries; j++) {
                *len += seg_header->segRef.entries[j].consec_chunks;
            }
        } else {
            *len = f->chunks[i]->chunk_count;
        }
    }

    return EXIT_SUCCESS;
}

/*
 * Copies a list of ids into buf.
 * The caller MUST allocate the correct amount before calling.
 * use gff_get_resource_length to know the amt!
 * returns -1 if there is an error, otherwise returns the number of ids copied in.
 */
//size_t gff_get_resource_ids(int idx, int type_id, unsigned int *ids) {
extern int gff_get_resource_ids(gff_file_t *f, int type_id, unsigned int *ids, uint32_t *pos) {
    unsigned char *cptr;
    gff_seg_header_t  *seg_header;
    gff_chunk_header_t *chunk_header;
    *pos = 0;
    
    for (int i = 0; i < f->num_types; i++) {
        if ((f->chunks[i]->chunk_type & GFFMAXCHUNKMASK) != type_id) { continue; }
        if (f->chunks[i]->chunk_count & GFFSEGFLAGMASK) {
            seg_header = (gff_seg_header_t*)&(f->chunks[i]->chunks[0]);
            for (int j = 0; j < seg_header->segRef.num_entries; j++) {
                for (int id_offset = 0; id_offset < seg_header->segRef.entries[j].consec_chunks; id_offset++) {
                    ids[*pos] = seg_header->segRef.entries[j].first_id + id_offset;
                    (*pos)++;
                }
            }
        } else {
            //TODO: migrate the cpt to the proper structs.
            cptr = (void*)f->chunks[i];
            for (int j = 0; j < f->chunks[i]->chunk_count; j++) {
                chunk_header = (void*)(cptr + sizeof(gff_chunk_list_header_t) + (j * sizeof(gff_chunk_header_t)));
                ids[*pos] = chunk_header->id;
                (*pos)++;
            }
        }
    }

    return EXIT_SUCCESS;
}

extern int gff_read_raw(gff_file_t *f, int gff_type, int res_id, char *buf, size_t len) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(f, &chunk, gff_type, res_id)) {
        goto header_error;
    }

    if (chunk.length > len) {
        goto overflow_error;
    }

    if (gff_read_chunk(f, &chunk, buf, len) != chunk.length) {
        goto read_error;
    }

    return chunk.length;
read_error:
overflow_error:
header_error:
    return 0;
}

extern size_t gff_read_raw_allocate(gff_file_t *f, int gff_type, int res_id, char **buf) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(f, &chunk, gff_type, res_id)) {
        goto header_error;
    }

    *buf = malloc(chunk.length);

    if (buf == NULL) {
        goto memory_error;
    }

    if (gff_read_chunk(f, &chunk, *buf, chunk.length) != chunk.length) {
        goto read_error;
    }

    return chunk.length;
read_error:
    free(*buf);
memory_error:
header_error:
    return 0;
}


extern int gff_read_text(gff_file_t *f, int res_id, char *text, size_t len) {
    int amt_read = gff_read_raw(f, GFF_TEXT, res_id, text, len);

    if (amt_read == 0) {
        goto read_error;
    }

    text[amt_read - 2] = '\0';

    return EXIT_SUCCESS;
read_error:
    return EXIT_FAILURE;
}

extern int gff_read_spin(gff_file_t *f, int res_id, char *text, size_t len) {
    int amt_read = gff_read_raw(f, GFF_SPIN, res_id, text, len);

    if (amt_read == 0) {
        goto read_error;
    }

    text[amt_read - 2] = '\0';

    return EXIT_SUCCESS;
read_error:
    return EXIT_FAILURE;
}

extern int gff_read_rdat(gff_file_t *f, int res_id, char *text, size_t len) {
    int amt_read = gff_read_raw(f, GFF_RDAT, res_id, text, len - 1);

    if (amt_read == 0) {
        goto read_error;
    }

    text[amt_read - 2] = '\0';

    return EXIT_SUCCESS;
read_error:
    return EXIT_FAILURE;
}

/*
 * Returns a list of ALL ids associated with file(idx) and type_id.
 * To know the length, please use gff_get_resource_length.
 */
//extern unsigned int* gff_get_id_list(int idx, int type_id) {
extern unsigned int* gff_get_id_list(gff_file_t *f, int type_id, uint32_t *len) {
    unsigned int *ids = NULL;
    uint32_t pos;

    if (!f) { return NULL; }

    if (gff_get_resource_length(f, type_id, len) == EXIT_FAILURE) {
        goto dne;
    }

    ids = malloc(sizeof(unsigned int) * *len);

    if (ids == NULL) { return NULL; }

    gff_get_resource_ids(f, type_id, ids, &pos);

    return ids;
dne:
    *len = 0;
    return NULL;
}

extern ssize_t gff_get_palette_id(gff_file_t *f, int palette_num) {
    if (!f || palette_num < 0 || palette_num >= f->num_palettes) {
        return -1;
    }

    return f->start_palette_index + palette_num;
}

extern ssize_t gff_get_palette_len(gff_file_t *f) {
    if (!f || !f->file) { return -1;}

    return f->num_palettes;
}

void gff_cleanup() {
    for (int i = 0; i < NUM_FILES; i++) {
        gff_close(i);
    }
}

static int gff_close_file(gff_file_t *gff) {
    if (gff->filename) {
        free(gff->filename);
        gff->filename = NULL;
    }
    //if (gff->map) {
        //free(gff->map);
        //gff->map = NULL;
    //}
    if (gff->chunks) {
        for (int i = 0; i < gff->num_types; i++) {
            if (gff->chunks[i]) {
                free(gff->chunks[i]);
                gff->chunks[i] = NULL;
            }
        }
        free(gff->chunks);
        gff->chunks = NULL;
    }
    if (gff->file) {
        fclose(gff->file);
    }
    if (gff->entry_table) {
        free(gff->entry_table);
        gff->entry_table = NULL;
    }
    gff->file = NULL;
    gff->filename = NULL;

    return EXIT_SUCCESS;
}

void gff_close (int gff_file) {
    if (gff_file < 0 || gff_file > NUM_FILES) { return; }

    gff_close_file(open_files + gff_file );

}
