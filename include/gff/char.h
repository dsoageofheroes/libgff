#ifndef GFF_CHAR_H
#define GFF_CHAR_H

#include <stdint.h>
#include <gff/psionic.h>
#include <gff/rdff.h>

//#include "wizard.h"
//#include "entity.h"
//#include "object.h"

typedef struct gff_char_entry_s {
    gff_rdff_header_t header;
    uint8_t data[];
    //ds_character_t pc;
    //gff_psin_t psi;
    //gff_spell_list_t spells;
    //gff_psionic_list_t psionics;
} gff_char_entry_t;

/*
extern int gff_char_delete(const int id);
extern int gff_char_add_character(gff_entity_t *pc, gff_psin_t *psi, gff_spell_list_t *spells, gff_psionic_list_t *psionics, char *name);
*/

#endif
