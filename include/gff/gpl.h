/* Game Programming Language (GPL) copyright SSI
 * This is an implementation of the Game Programming Lanugage
 * developed at SSI.
 */

#ifndef GFF_GPL_H
#define GFF_GPL_H

#include "debug.h"
#include "status.h"
#include "gfftypes.h"
#include "scmd.h"

#define gpl_check_index_t uint16_t
#define NULL_CHECK (0xFFFF)

#define MAX_PARAMETERS (8)
typedef struct gff_gpl_param_s {
    int32_t val[MAX_PARAMETERS];
    int32_t *ptr[MAX_PARAMETERS];
} gff_gpl_param_t;

enum gff_gpl_global_e {
    GPL_POV,
    GPL_OTHER
};

extern gff_gpl_param_t param;

extern gff_status_t gff_gpl_init();
extern gff_status_t gff_gpl_change_region(const int region_id);
extern gff_status_t gff_gpl_write_local_state(FILE *file);
extern gff_status_t gff_gpl_write_global_state(FILE *file);

extern gff_status_t gff_gpl_cleanup();

extern gff_status_t gff_gpl_request_impl(int16_t token, int16_t name,
        int32_t num1, int32_t num2);

/* Begin Parsing functions */ 
extern gff_status_t gff_gpl_peek_one_byte(uint8_t *d);
extern gff_status_t gff_gpl_get_byte(uint8_t *d);
/* End Parsing functions */ 

extern gff_status_t gpl_lua_debug();
extern gff_status_t gpl_set_exit();

extern void gpl_print_next_bytes(int amt);

extern uint8_t command_implemented; // Temporary while I figure out each function.

#endif
