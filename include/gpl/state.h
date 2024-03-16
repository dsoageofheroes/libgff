#ifndef GPL_STATE_H
#define GPL_STATE_H

#include <stdint.h>

#define GPL_RANGE_MAX (999)

typedef enum gpl_gnum_e {
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
} gpl_gnum_t;

extern int gff_gpl_state_init();
extern int gff_gpl_state_cleanup();
//extern int gff_gpl_state_register(lua_State *l);

extern int gff_gpl_local_clear();
extern int gff_gpl_deserialize_globals(char *buf);
extern int gff_gpl_deserialize_locals(char *buf);
extern int gff_gpl_state_debug();
extern int gff_gpl_set_gname(const gpl_gnum_t index, const int32_t obj);

extern int gff_gpl_get_gname(const gpl_gnum_t pos, int16_t *gn);

extern int gff_gpl_serialize_globals(uint32_t *len, char **ret);
extern int gff_gpl_serialize_locals(uint32_t *len, char **ret);


#endif
