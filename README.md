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

To get an entry you must find the correct resource id (available entries can be queried with 

Each type of supported entry can be read in with the respective `gff_read_*`/`gff_load_*` functions.
The `gff_read_*` will read into buffer you, the developer, must provide. 
`gff_load_*` will allocate the buffer, via `malloc`, and return. Note that `gff_read_*` function will fail if the buffer is too small. 
Similiarly the `gff_load_*` function will fail in the extremely rare event that `malloc` somehow failed.





# gffmod

To be written

### Writing to GFF Files

Currently there is no plan to write to a gff file. If there is a desire to write to these file, please message me on Discord.
