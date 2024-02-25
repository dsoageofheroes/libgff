#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gpl/gpl.h>
//#include "wizard.h"
//#include "gpl-manager.h"
//#include "ssi-item.h"
//#include "region-manager.h"
//#include "gpl-state.h"
//#include "gpl-string.h"
//#include "gpl-var.h"
//#include "player.h"
//#include "trigger.h"

//#define DSL_ORDER     (0)
//#define DSL_LOS_ORDER (1)

//uint8_t command_implemented = 0; // Temporary while I figure out each function.

/* Globals */
//void get_parameters(int16_t amt);
//static gff_entity_t *other = NULL;

//gff_gpl_param_t param;
/* End Globals */

/*
extern gff_status_t gff_gpl_get_global(enum gff_gpl_global_e what, gff_entity_t **ent) {
    if (!ent) { return GFF_NULL_ARGUMENT; }

    switch(what) {
        case GPL_OTHER: *ent = other; return GFF_SUCCESS;
        default: warn("unknown type: %d\n", what); break;
    }

    return GFF_GPL_UNKNOWN_TYPE;
}

extern gff_status_t gff_gpl_set_global(enum gff_gpl_global_e what, gff_entity_t *entity) {
    switch(what) {
        case GPL_OTHER: other = entity; return GFF_SUCCESS;
        default: warn("unknown type: %d\n", what); break;
    }
    return GFF_GPL_UNKNOWN_TYPE;
}

extern gff_status_t gff_gpl_change_region(const int region_id) {
    gff_region_t *reg;
    //gpl_execute_subroutine(region_id, 0, 1);
    gff_region_manager_get_region(region_id, 0, &reg);
    return gff_gpl_lua_execute_script(region_id, 0, 1);
}
*/

static void initialize_gpl_stack() {
    //gff_gpl_global_strings = (gff_gpl_string_t*) malloc(GSTRINGVARSIZE);
    //memset(gff_gpl_global_strings, 0, GSTRINGVARSIZE);

    //gff_gpl_local_strings  = (gff_gpl_string_t*) malloc(LSTRINGVARSIZE);
    //memset(gff_gpl_local_strings, 0, LSTRINGVARSIZE);

    //gff_gpl_global_string  = (uint8_t*)malloc(TEXTSTRINGSIZE);
}

extern int gff_gpl_init() {
    info("Initalizing GPL Engine.\n");
    initialize_gpl_stack();
    //gff_gpl_init_vars();
    //gff_player_init();
    //ssi_item_init();
    //gff_trigger_init();
    //gff_gpl_manager_init();
    //gff_region_manager_init();
    return EXIT_SUCCESS;
}

extern int gff_gpl_cleanup() {
    //gff_powers_cleanup();
    //free(gff_gpl_global_string);
    //free(gff_gpl_local_strings);
    //free(gff_gpl_global_strings);
    //gff_gpl_cleanup_vars();
    //gff_gpl_manager_cleanup();
    //ssi_item_close();
    //gff_trigger_cleanup();
    return EXIT_SUCCESS;
}
