#ifndef GPL_MANAGER_H
#define GPL_MANAGER_H

#include <stdlib.h>
#include <stdint.h>

extern int gff_gpl_manager_init();
extern int gff_gpl_manager_cleanup();
extern int gff_gpl_lua_execute_script(size_t file, size_t addr, uint8_t is_mas);
extern int gff_gpl_execute_string(const char *str);
extern int gff_gpl_lua_load_all_scripts();
extern int gff_gpl_lua_debug();
extern int gff_gpl_push_context();
extern int gff_gpl_pop_context();
extern int gff_gpl_set_exit();
extern int gff_gpl_in_exit();

#endif
