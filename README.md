# libgff

`libgff` is designed to read and write from GFF file from old SSI games.
Some of the work is reversed engineered from the .gff others parts are scoured from resources on the internet. Sorry to all those I wasn't able to give a citation.

`libgff` will provide two tools: 
 * `libgff`: a C library for reading gff files with support for converting to modern formats. This include text, spells, images, maps information, video, and hooks for interfacing with GPL.
 * `gfftool`: a CLI tool for reading and extracting components of gff files.


# Programming Guide

## Initial Example

Lets start with a simple program:
```
#include <stdio.h>
#include <gff/gff.h>

int main(int argc, char *argv[]) {
    gff_file_t gff;

    if (gff_open(&gff, argv[1])) {
        fprintf(stderr, "Unable to open '%s'.\n", argv[1]);
        return 1;
    }

    gff_close(&gff);

    return 0;
}
```

To compile you will need the include gff files and then link BOTH libgff (`-lgff`) and xmi2mid (`-lxmi2mid`).

To see a list of entries in the gff:

```
#include <stdio.h>
#include <gff/gff.h>
#include <gff/gfftypes.h>

int main(int argc, char *argv[]) {
    gff_file_t gff;

    if (gff_open(&gff, argv[1])) {
        fprintf(stderr, "Unable to open '%s'.\n", argv[1]);
        return 1;
    }

    for (int i = 0; i < gff.num_types; i++) {
        gff_chunk_entry_t *entry = gff.chunks[i];
        printf("Entry %2d: '%c%c%c%c' %s\n", i, 
                entry->chunk_type,
                entry->chunk_type >> 8,
                entry->chunk_type >> 16,
                entry->chunk_type >> 24,
                gff_type_to_str(entry->chunk_type)
              );
    }

    gff_close(&gff);

    return 0;
}
```

## Loading Individual Entries

Each entry of the GFF has a type (e.g. `BMP`, `ICON`, `GPL`, `WIND`, etc..) and an identification number call a resource id.
Therefore, to get an entry you must:

1. Determine the type (see previous code) and find the correct resource id (available entries can be queried with `gff_load_id_list`.)
2. load or read the correct entry.


### Acquiring the Resource Ids

Sometimes you will know the resource id already.
Other times you can iterate with `gff_load_id_list`:
```
    uint32_t *ids, len;
    if (gff_load_id_list(&gff, GFF_SPIN, &ids, &len)) {
        fprintf(stderr, "Unable to load ids.\n");
        return 1;
    }

    printf("GFF_SPIN Resource ids: ");
    for (int i = 0; i < len; i++) {
        printf("%d ", ids[i]);
    }
    printf("\n");
```

### Loading the data.

Each type of supported entry can be read in with the respective `gff_read_*`/`gff_load_*` functions.
The `gff_read_*` will read into buffer that you, the developer, must provide. 
`gff_load_*` will allocate the buffer, via `malloc`, and return. YOU MUST FREE THE BUFFER!
Note that `gff_read_*` function will fail if the buffer is too small. 
Similiarly the `gff_load_*` function will fail in the extremely rare event that `malloc` somehow failed.

Here is an example of reading from a `GFF_SPIN`.
```
    char spin[1024];

    if (gff_read_spin(&gff, 1000, spin, 1024)) {
        fprintf(stderr, "Unable to read SPIN");
        return 1;
    }

    printf("spin: %s\n", spin);
```

A load would be:
```
    char *spin;
    uint32_t len;

    if (gff_load_spin(&gff, 1000, &spin, &len)) {
        fprintf(stderr, "Unable to read SPIN");
        return 1;
    }

    printf("spin: %s\n", spin);

    free(spin);
```

Keep in minds that some targets have a special type, like `GFF_ETAB` which specify the map objects.
Here is the list:
```
// Read Functions
extern size_t             gff_read_chunk_piece(gff_file_t *f, gff_chunk_header_t *chunk, void *read_buf, const size_t len);
extern int                gff_read_raw(gff_file_t *f, int gff_type, int res_id, uint8_t *buf, size_t len);
extern int                gff_read_text(gff_file_t *f, int res_id, char *text, size_t len);
extern int                gff_read_mas(gff_file_t *f, int res_id, uint8_t *text, size_t len);
extern int                gff_read_names(gff_file_t *f, int res_id, char *names, size_t len, uint32_t *amt);
extern int                gff_read_rdat(gff_file_t *f, int res_id, char *text, size_t len);
extern int                gff_read_spin(gff_file_t *f, int res_id, char *text, size_t len);
extern int                gff_read_merr(gff_file_t *f, int res_id, char *text, size_t len);
extern int                gff_read_region_flags(gff_file_t *gff, int res_id, uint8_t flags[MAP_ROWS][MAP_COLUMNS]);
extern int                gff_read_global_flags(gff_file_t *gff, int res_id, uint8_t flags[MAP_ROWS][MAP_COLUMNS]);

// Load Functions
extern size_t             gff_load_raw(gff_file_t *f, int gff_type, int res_id, uint8_t **buf);
extern int                gff_load_id_list(gff_file_t *f, int type_id, uint32_t **ids, uint32_t *len);
extern int                gff_load_mas(gff_file_t *f, int res_id, uint8_t **mas, size_t *len);
extern int                gff_load_gpl(gff_file_t *f, int res_id, uint8_t **gpl, size_t *len);
extern int                gff_load_monster_list(gff_file_t *f, int res_id, gff_monster_list_t **monr, size_t *len);
extern int                gff_load_spin(gff_file_t *f, int res_id, char **text, uint32_t *len);
extern int                gff_load_etab(gff_file_t *gff, int res_id, gff_etab_object_t **etab, uint32_t *num_etabs);
```

### Images

Images are stored with palettes.
To support modern image formats, there is a DarkSun to rgba converter:

TBW

### Music

Music is also stored in XMI format which is close, but not exactly, MIDI.

TBW

### GPL

TBW

# gffmod

To be written

### Writing to GFF Files

Currently there is no plan to write to a gff file. If there is a desire to write to these file, please message me on Discord.
