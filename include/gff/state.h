#ifndef GFF_GPL_STATE_H
#define GFF_GPL_STATE_H

#include "lua.h"
#include <stdint.h>

#define GPL_RANGE_MAX (999)

typedef enum gff_gpl_gnum_e {
    GNAME_FIGHT,
    GNAME_UNKNOWN1, // X
    GNAME_UNKNOWN2, // Y
    GNAME_UNKNOWN3, // Z
    GNAME_REGION,
    GNAME_UNKNOWN5, // Current player?
    GNAME_ACTIVE,
    GNAME_PASSIVE,
    GNAME_UNKNOWN8,
    GNAME_TIME, // confirmed this needs to be updated
    GNAME_UNKNOWNA,
    GNAME_UNKNOWNB,
    GNAME_UNKNOWNC,
    GNAME_NUM
} gff_gpl_gnum_t;

extern gff_status_t gff_gpl_state_init();
extern gff_status_t gff_gpl_state_cleanup();
extern gff_status_t gff_gpl_state_register(lua_State *l);

extern gff_status_t gff_gpl_local_clear();
extern gff_status_t gff_gpl_deserialize_globals(char *buf);
extern gff_status_t gff_gpl_deserialize_locals(char *buf);
extern gff_status_t gff_gpl_state_debug();
extern gff_status_t gff_gpl_set_gname(const gpl_gnum_t index, const int32_t obj);

extern gff_status_t gff_gpl_get_gname(const gpl_gnum_t pos, int16_t *gn);

extern gff_status_t gff_gpl_serialize_globals(uint32_t *len, char **ret);
extern gff_status_t gff_gpl_serialize_locals(uint32_t *len, char **ret);

#endif
