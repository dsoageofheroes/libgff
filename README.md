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
To load images first read the palettes from a file with `gff_palettes_read`.
Then load the number of frames with `gff_get_frame_count`.
Note that frame sometimes have different dimensions, load them with `gff_frame_info`.
Finally, each frame can be converted to rgba with `gff_load_frame_rgba`

    TS(gff_frame_info(f, GFF_ICON, ids[0], 0, &info));
```
extern int             gff_palettes_read(gff_file_t *f);
extern int             gff_get_frame_count(gff_file_t *f, int type_id, int res_id);
extern int             gff_frame_info(gff_file_t *f, int type_id, int res_id, int frame_id, gff_frame_info_t *info);
extern unsigned char*  gff_load_frame_rgba(gff_file_t *f, int type_id, int res_id, int frame_id, const gff_palette_t *pal);
```

Example:

```
...
#include <gff/image.h>
...

    if (gff_palettes_read(&gff)) {
        fprintf(stderr, "Unable to read palettes\n");
        return 1;
    }

    int frame_count = gff_get_frame_count(&gff, GFF_ICON, 2000);
    gff_frame_info_t info;

    for (int frame = 0; frame < frame_count; frame++) {
        gff_frame_info(&gff, GFF_ICON, 2000, frame, &info);
        printf("ICON 2000 frame %d, width: %d, height: %d\n", frame, info.w, info.h);
        unsigned char *data = gff_load_frame_rgba(&gff, GFF_ICON, 2000, frame, gff.pals->palettes);
        // data hold info.w * info.h * 4 bytes of data. Each pixel is 32-bit red, green, blue, alpha.
        free(data);
    }
...

```

### Music

Music is also stored in XMI format which is an extended version of MIDI.
Since XMI is not usually supported there are tools to convert to MIDI.

THere is one function for converting to midi:
```
extern int                gff_load_gseq(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len);
```

Example:
```
    uint8_t *data;
    uint32_t len;
    if (gff_load_gseq(&gff, 1, &data, &len)) {
        printf("Unable to read gseq!\n");
        return 1;
    }

    // data is now MIDI, length is len.

    free (data);
```

Note that if you wish to specify the type of conversion you can with:
```
extern int                gff_load_gseq_type(gff_file_t *gff, int res_id, uint8_t **data, uint32_t *len, int type):
```
Possible types are `XMIDI_CONVERT_NOCONVERSION`, `XMIDI_CONVERT_MT32_TO_GM`, `XMIDI_CONVERT_MT32_TO_GS`, `XMIDI_CONVERT_GS127_TO_GS`.
I've found `XMIDI_CONVERT_MT32_TO_GS` to be the closest and it is the default if type is not specified.

There are respective function for `pseq`, `gseq` and `lseq`


### Sound Effects

### GPL

TBW

# gfftool

To be written

### Writing to GFF Files

Currently there is no plan to write to a gff file. If there is a desire to write to these file, please message me on Discord.

