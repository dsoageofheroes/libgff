/* Game Programming Language (GPL) copyright SSI
 * This is an implementation of the Game Programming Lanugage
 * developed at SSI.
 */

#ifndef GPL_H
#define GPL_H

#include <gff/debug.h>
#include <gff/gff.h>
#include <gff/gfftypes.h>

//#define gpl_check_index_t uint16_t
#define NULL_CHECK (0xFFFF)

#define MAX_PARAMETERS (8)
typedef struct gff_gpl_param_s {
    int32_t  val[MAX_PARAMETERS];
    int32_t *ptr[MAX_PARAMETERS];
} gff_gpl_param_t;


//enum gff_gpl_global_e {
    //GPL_POV,
    //GPL_OTHER
//} gff_gpl_global_t;

//extern gff_gpl_param_t param;
typedef struct gff_gpl_transformer_s {
} gff_gpl_transformer_t;

typedef struct gpl_data_s {
    uint8_t *gpl;
    uint8_t *end;
    uint16_t depth;
    uint8_t is_mas;
    uint8_t in_retval;
    gff_gpl_transformer_t *ggt;
} gpl_data_t;

extern int gff_gpl_init();
extern int gff_gpl_cleanup();

extern int gff_gpl_parse(uint8_t *gpl, const size_t len, gff_gpl_transformer_t *ggt, uint8_t is_mas);
extern int gff_gpl_load(size_t file, size_t addr, uint8_t is_mas);
//extern gff_status_t gff_gpl_change_region(const int region_id);
//extern gff_status_t gff_gpl_write_local_state(FILE *file);
//extern gff_status_t gff_gpl_write_global_state(FILE *file);

//extern gff_status_t gff_gpl_set_global(enum gff_gpl_global_e what, gff_entity_t *entity);

//extern gff_status_t gff_gpl_request_impl(int16_t token, int16_t name,
        //int32_t num1, int32_t num2);
//extern gff_status_t gff_gpl_get_global(enum gff_gpl_global_e what, gff_entity_t **ent);

/* Begin Parsing functions */ 
extern int gff_gpl_peek_one_byte(gpl_data_t *gpl, uint8_t *d);
extern int gff_gpl_get_byte(gpl_data_t *gpl, uint8_t *b);
/* End Parsing functions */ 

//extern gff_status_t gpl_lua_debug();
//extern gff_status_t gpl_set_exit();

//extern void gpl_print_next_bytes(int amt);

//extern uint8_t command_implemented; // Temporary while I figure out each function.

#endif
