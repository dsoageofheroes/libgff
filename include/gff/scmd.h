// SCMD (Script CoMmanDs)
#ifndef GFF_SCMD_H
#define GFF_SCMD_H

#include "common.h"

typedef struct gff_scmd_s {
    uint8_t bmp_idx; // 0-254 = valid, 255 = none
    uint8_t delay;   // 0-255 = valid, ticks are 96 per second
    uint8_t flags;   // See SCMD flags
    int8_t xoffset; // change in x for new bmp
    int8_t yoffset; // change in x for new bmp
    int8_t xoffsethot; // change in x for new bmp
    int8_t yoffsethot; // change in x for new bmp
    uint8_t soundidx;  // sound index to play
} gff_scmd_t;

typedef struct gff_ojff_s {
    uint16_t flags;
    int16_t  xoffset;
    int16_t  yoffset;
    uint16_t xpos;
    uint16_t ypos;
    int8_t   zpos;
    uint8_t  object_index;
    uint16_t bmp_id;
    uint16_t script_id;
} gff_ojff_t;

// SCMD flags
#define SCMD_JUMP (0x01) // loop
#define SCMD_LAST (0x02) // end of script
#define SCMD_XMIRROR (0x04) // mirror x?
#define SCMD_YMIRROR (0x08) // mirror y?
#define SCMD_MOVING (0x10) // is moving?
#define SCMD_COMBAT (0x20) // load combat image.
#define SCMD_OK_HOT (0x40) // Don't change the hot
//#define SCMD_MAX      (256)
#define SCMD_MAX      (1<<10)
#define SCMD_MAX_SIZE (36)
#define SCMD_DEFAULT_DELAY (16)
#define SCMD_TICKS_PER_SECOND (96)

extern int gff_scmd_read(gff_file_t *f, const int res_id, gff_scmd_t **scmd);
extern int gff_scmd_next_pos(const gff_scmd_t *scmd, const int scmd_index);
extern int gff_scmd_total_delay(const gff_scmd_t *scmd, int scmd_index);

// Okay these break the whole always return SUCCESS/FAILURE, but this is just a helper...
extern gff_scmd_t* gff_scmd_get(gff_scmd_t *scmd_entry, const int index);
extern gff_scmd_t* gff_scmd_empty();

#endif
