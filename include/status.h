#ifndef GFF_STATUS_H
#define GFF_STATUS_H

typedef enum gff_status_e {
    GFF_OK,
    GFF_NOT_IMPLEMENTED,
    GFF_DNE, // Does not exist
    GFF_NO_LENGTH, // Could not get the length of the resource.
    GFF_NO_TYPE, // Could not find the type
} gff_status_t;

#endif
