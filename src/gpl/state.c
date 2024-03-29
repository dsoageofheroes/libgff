#include <gpl/gpl.h>
#include <gpl/state.h>
/*
//#include "lua-inc.h"
#include "gpl-manager.h"
//#include "region.h"
#include "gpl-state.h"
#include "gameloop.h"
#include "port.h"
#include "map.h"
#include "ssi-scmd.h"
#include "region-manager.h"
#include "trigger.h"
#include "player.h"
#include "narrate.h"
#include "sprite.h"
*/
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE           (1<<12)
#define MAX_GFLAGS         (800)
#define MAX_LFLAGS         (64)
#define MAX_GNUMS          (400)
#define MAX_GBIGNUMS       (40)
#define MAX_LBIGNUMS       (40)
#define MAX_LNUMS          (32)
#define MAX_GSTRS          (32)
#define STRING_SIZE        (1024)
#define MAX_GNAMES         (13)

static int8_t  gpl_global_flags[MAX_GFLAGS];
static int8_t  gpl_local_flags[MAX_LFLAGS];
static int16_t gpl_global_nums[MAX_GNUMS];
static int16_t gpl_local_nums[MAX_LNUMS];
static int32_t gpl_global_bnums[MAX_GBIGNUMS];
static int32_t gpl_local_bnums[MAX_LBIGNUMS];
static char    gpl_global_strs[MAX_GSTRS][STRING_SIZE];
static int16_t gpl_gnames[MAX_GNAMES];

extern int gff_gpl_state_init() {
    memset(gpl_global_flags, 0x0, sizeof(int8_t) * MAX_GFLAGS);
    memset(gpl_global_nums, 0x0, sizeof(int16_t) * MAX_GNUMS);
    memset(gpl_global_bnums, 0x0, sizeof(int32_t) * MAX_GBIGNUMS);
    memset(gpl_global_strs, 0x0, sizeof(char) * MAX_GSTRS * STRING_SIZE);
    memset(gpl_gnames, 0x0, sizeof(int16_t) * MAX_GNAMES);
    return gff_gpl_local_clear();
}

extern int gff_gpl_state_cleanup() {
    return EXIT_SUCCESS;
}

extern int gff_gpl_state_debug() {
    const size_t max_print = 10;

    printf("GF: %d", gpl_global_flags[0]);
    for (size_t i = 1; i < MAX_GFLAGS && i < max_print; i++) {
        printf(", %d", gpl_global_flags[i]);
    }
    printf("\n");

    printf("LL: %d", gpl_local_flags[0]);
    for (size_t i = 1; i < MAX_LFLAGS && i < max_print; i++) {
        printf(", %d", gpl_local_flags[i]);
    }
    printf("\n");

    printf("GN: %d", gpl_global_nums[0]);
    for (size_t i = 1; i < MAX_GNUMS && i < max_print; i++) {
        printf(", %d", gpl_global_nums[i]);
    }
    printf("\n");

    printf("LN: %d", gpl_local_nums[0]);
    for (size_t i = 1; i < MAX_LNUMS && i < max_print; i++) {
        printf(", %d", gpl_local_nums[i]);
    }
    printf("\n");

    printf("GBN: %d", gpl_global_bnums[0]);
    for (size_t i = 1; i < MAX_GBIGNUMS && i < max_print; i++) {
        printf(", %d", gpl_global_bnums[i]);
    }
    printf("\n");

    printf("LBN: %d", gpl_local_bnums[0]);
    for (size_t i = 1; i < MAX_LBIGNUMS && i < max_print; i++) {
        printf(", %d", gpl_local_bnums[i]);
    }
    printf("\n");

    printf("GSTRS: %s", gpl_global_strs[0]);
    for (size_t i = 1; i < MAX_GSTRS && i < max_print; i++) {
        printf(", '%s'", gpl_global_strs[i]);
    }
    printf("\n");

    printf("GNAMES: %d", gpl_gnames[0]);
    for (size_t i = 1; i < MAX_GNAMES && i < max_print; i++) {
        printf(", %d", gpl_gnames[i]);
    }
    printf("\n");
    return EXIT_SUCCESS;
}

// TODO: this will need to be attached to a region.
extern int gff_gpl_write_local_state(FILE *file) {
    for (int i = 0; i < MAX_LFLAGS; i++) {
        fprintf(file, "gpl.set_lf(%d, %d)\n", i, gpl_local_flags[i]);
    }

    for (int i = 0; i < MAX_LNUMS; i++) {
        fprintf(file, "gpl.set_ln(%d, %d)\n", i, gpl_local_nums[i]);
    }

    for (int i = 0; i < MAX_LBIGNUMS; i++) {
        fprintf(file, "gpl.set_lbn(%d, %d)\n", i, gpl_local_bnums[i]);
    }
    return EXIT_SUCCESS;
}

extern int gff_gpl_write_global_state(FILE *file) {
    for (int i = 0; i < MAX_GFLAGS; i++) {
        fprintf(file, "gpl.set_gf(%d, %d)\n", i, gpl_global_flags[i]);
    }

    for (int i = 0; i < MAX_GNUMS; i++) {
        fprintf(file, "gpl.set_gn(%d, %d)\n", i, gpl_global_nums[i]);
    }

    for (int i = 0; i < MAX_GBIGNUMS; i++) {
        fprintf(file, "gpl.set_gbn(%d, %d)\n", i, gpl_global_bnums[i]);
    }

    for (int i = 0; i < MAX_GNAMES; i++) {
        fprintf(file, "gpl.set_gname(%d, %d)\n", i, gpl_gnames[i]);
    }

    for (int i = 0; i < MAX_GSTRS; i++) {
        fprintf(file, "gpl.set_gstr(%d, \"%s\")\n", i, gpl_global_strs[i]);
    }
    return EXIT_SUCCESS;
}

/*
static int set_while_callback(lua_State *l) {
    //const char *val = luaL_checkstring(l, 2);
    //lua_Integer state = luaL_checkinteger(l, 1);
    return 0;
}

// Public to C library
extern int gff_gpl_set_gname(const gpl_gnum_t index, const int32_t obj) {
    if (index < 0 || index > MAX_GNAMES) { return EXIT_FAILURE; }
    //printf("GNAME----------------------------------------------------->[%d] = %d\n", index, obj);
    gpl_gnames[index] = obj;
    return EXIT_SUCCESS;
}

// Public to LUA
static int set_gf(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_GFLAGS) {
        printf("ERROR: " PRI_LI " is out of range for global flags!\n", id);
        exit(1);
    }
    gpl_global_flags[id] = val;
    debug("gpl_globals_flags[" PRI_LI "] = " PRI_LI "\n", id, val);
    return 0;
}

static int get_gf(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    //printf("get_gf(%lld) = %d\n", id, gpl_global_flags[id]);
    if (id < 0 || id >= MAX_GFLAGS) {
        printf("ERROR: " PRI_LI " is out of range for global flags!\n", id);
        exit(1);
    }
    //printf("get_gf(%d) = %d\n", id, gpl_global_flags[id]);
    lua_pushinteger(l, gpl_global_flags[id]);
    return 1;
}

static int set_lf(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_LFLAGS) {
        printf("ERROR: " PRI_LI " is out of range for local flags!\n", id);
        exit(1);
    }
    gpl_local_flags[id] = val;
    //debug("gpl_local_flags[" PRI_LI "] = " PRI_LI "\n", id, val);
    return 0;
}

static int get_lf(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0 || id >= MAX_LFLAGS) {
        printf("ERROR: " PRI_LI " is out of range for local flags!\n", id);
        exit(1);
    }
    //printf("local_flag[" PRI_LI "] = %d (get)\n", id, gpl_local_flags[id]);
    lua_pushinteger(l, gpl_local_flags[id]);
    return 1;
}

static int set_gn(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_GNUMS) {
        printf("ERROR: " PRI_LI " is out of range for global nums!\n", id);
        exit(1);
    }
    gpl_global_nums[id] = val;
    debug("gpl_globals_nums[" PRI_LI "] = " PRI_LI "\n", id, val);
    return 0;
}

static int get_gn(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0 || id >= MAX_GNUMS) {
        printf("ERROR: " PRI_LI " is out of range for global nums!\n", id);
        exit(1);
    }
    debug("get gpl_globals_nums[" PRI_LI "] : " PRI_LI "\n", id, gpl_global_nums[id]);
    lua_pushinteger(l, gpl_global_nums[id]);
    return 1;
}

static int set_ln(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_LNUMS) {
        printf("ERROR: " PRI_LI " is out of range for local nums!\n", id);
        exit(1);
    }
    gpl_local_nums[id] = val;
    return 0;
}

static int get_ln(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0 || id >= MAX_LNUMS) {
        printf("ERROR: " PRI_LI " is out of range for local nums!\n", id);
        exit(1);
    }
    lua_pushinteger(l, gpl_local_nums[id]);
    return 1;
}

static int set_gbn(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_GBIGNUMS) {
        error("ERROR: " PRI_LI " is out of range for global big nums!\n", id);
        exit(1);
    }
    gpl_global_bnums[id] = val;
    return 0;
}

static int get_gbn(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0 || id >= MAX_GBIGNUMS) {
        error("ERROR: " PRI_LI " is out of range for global big nums!\n", id);
        exit(1);
    }
    lua_pushinteger(l, gpl_global_bnums[id]);
    return 1;
}

static int set_lbn(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_LBIGNUMS) {
        printf("ERROR: " PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    gpl_local_bnums[id] = val;
    return 0;
}

static int get_lbn(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0 || id >= MAX_LBIGNUMS) {
        printf("ERROR: " PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    lua_pushinteger(l, gpl_local_bnums[id]);
    return 1;
}

static int set_gstr(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    const char *val = luaL_checkstring(l, 2);
    if (id < 0 || id >= MAX_GSTRS) {
        printf("ERROR: " PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    strncpy(gpl_global_strs[id], val, STRING_SIZE);
    return 0;
}

static int get_gstr(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0 || id >= MAX_GSTRS) {
        printf("ERROR: " PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    lua_pushstring(l, gpl_global_strs[id]);
    return 1;
}

static int get_gname(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    int16_t gn;
    if (id < 0 || id >= MAX_GNAMES) {
        printf("ERROR: " PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    //printf("GNAME----------------------------------------------------->[" PRI_LI "] => %d\n", id, gpl_get_gname(id));
    gff_status_check(gff_gpl_get_gname(id, &gn), "Unable to get gname.");
    lua_pushnumber(l, gn);
    return 1;
}

static int get_pov(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    if (id < 0) {
        printf("ERROR: " PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    //printf("GNAME----------------------------------------------------->[" PRI_LI "] = %d\n", id, gpl_gnames[id]);
    warn("get_pov (%d): not implemented!\n", (int)id);
    lua_pushnumber(l, 0);
    return 1;
}

static int set_gname(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    lua_Integer val = luaL_checkinteger(l, 2);
    if (id < 0 || id >= MAX_GNAMES) {
        error("" PRI_LI " is out of range for local big nums!\n", id);
        exit(1);
    }
    if (id < 0 || id >= GNAME_NUM) {
        error("illegal set_gname? id = " PRI_LI "\n", id);
    } else {
        gpl_gnames[id] = val;
    }
    //printf("GNAME----------------------------------------------------->[" PRI_LI "] <= %d\n", id, gpl_gnames[id]);
    return 0;
}

static int get_type(lua_State *l) {
    //lua_Integer id = luaL_checkinteger(l, 1);
    error("error: gpl.get_type: not implemented returning -1!\n");
    //lua_pushnumber(l, gpl_gnames[id]);
    lua_pushnumber(l, -1);
    return 1;
}

static int get_id(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer obj_name = luaL_checkinteger(l, 2);
    //printf("!!!!!!!!!!gpl.get_name: not implemented returning -1!\n");
    //printf("-------->returning " PRI_LI "\n", obj);
    //error("error: gpl.get_id not implement, just returning obj (" PRI_LI " ).\n", obj);
    printf("get_id(%lld, %lld) -- %lld\n", obj, obj_name, obj_name & 0x7FFF);
    lua_pushnumber(l, obj);
    //lua_pushnumber(l, -1);
    return 1;
}

static int get_element_entity(lua_State *l, gff_dude_t *dude, const int depth, const int element) {
    if (!dude) {
        warn("passed a null entity! (depth = %d, element = %d)\n", depth, element);
        lua_pushnumber(l, -1);
        return 1;
    }

    if (depth == 1 && element == 19) { // Name
        lua_pushstring(l, dude->name);
        return 1;
    }

    // right now just return the id!
    //printf("get_element_entity: need to get %d, %d\n", depth, element);
    lua_pushnumber(l, dude->ds_id);
    return 1;
}

static int get_element(lua_State *l) {
    gff_entity_t *ent, *active;
    lua_Integer obj_name = luaL_checkinteger(l, 1);
    lua_Integer header   = luaL_checkinteger(l, 2);
    lua_Integer depth    = luaL_checkinteger(l, 3);
    lua_Integer element  = luaL_checkinteger(l, 4);
    //printf("get_element(%lld, %lld, %lld, %lld) -- %lld\n", obj_name, header, depth, element, obj_name & 0x7FFF);
    switch(obj_name & 0x7FFF) {
        case 0x25: // POV -- active char?
            gff_player_get_active(&active);
            return get_element_entity(l, active, depth, element);
        case 0x28: // OTHER
            gff_gpl_get_global(GPL_OTHER, &ent);
            return get_element_entity(l, ent, depth, element);
    }
    //lua_pushnumber(l, obj);
    warn("get_element(%lld, %lld, %lld, %lld) -- %lld, failed to find\n", obj_name, header, depth, element, obj_name & 0x7FFF);
    lua_pushnumber(l, -1);
    return 1;
}

static int set_element(lua_State *l) {
    lua_Integer obj_name = luaL_checkinteger(l, 1);
    lua_Integer header   = luaL_checkinteger(l, 2);
    lua_Integer depth    = luaL_checkinteger(l, 3);
    lua_Integer element  = luaL_checkinteger(l, 4);
    lua_Integer accum    = luaL_checkinteger(l, 4);

    warn("set_element(%lld, %lld, %lld, %lld) -- %lld <- %lld is unimplemented.\n", obj_name, header, depth, element, obj_name & 0x7FFF, accum);
    return 0;
}

// This is kinda wierd as each object would need an id. But I don't see how that currently would work.
// So, players have negative numbers.
static int get_party(lua_State *l) {
    lua_Integer member = luaL_checkinteger(l, 1);
    //static int idx = -1;
    //error("error: gpl.get_party: not implemented returning 9999 for member: " PRI_LI "!\n", member);
    if (gff_player_exists(member) != EXIT_SUCCESS) {
        debug("Party member does not exist in slot: " PRI_LI ", returning 9999!\n", member);
        lua_pushnumber(l, 9999);
        return 1;
    }

    // For now just return the negative of it.
    //lua_pushnumber(l, gpl_gnames[id]);
    //lua_pushnumber(l, 9999);
    lua_pushnumber(l, -member);
    //lua_pushnumber(l, idx);
    //if (idx == 9999) { idx = -1; return 1;}
    //idx--;
    //if (idx < -4) {idx = 9999;};
    return 1;
}

static int is_true(lua_State *l) {
    if (lua_isinteger(l, 1)) {
        //printf("------------>integer = %lld\n", lua_tointeger(l, 1));
        lua_pushboolean(l, lua_tointeger(l, 1) != 0);
    } else if (lua_isboolean(l, 1)) {
        //printf("------------>boolean = %d, => %d\n", lua_toboolean(l, 1), lua_toboolean(l, 1) == 1);
        lua_pushboolean(l, lua_toboolean(l, 1) == 1);
    } else {
        error("ERROR: did not received a boolean or int for accum testing!\n");
        lua_pushboolean(l, 0);
    }

    return 1;
}

static int gpl_getX(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    gff_region_t *reg;
    gff_dude_t *dude = NULL;

    gff_region_manager_get_current(&reg);
    gff_entity_list_for_each(reg->entities, dude) {
        if (dude->ds_id == id) {
            lua_pushnumber(l, dude->mapx - (dude->anim.xoffset + 15) / 16); // the +15 forces a round up.
            return 1;
        }
    }

    lua_pushnumber(l, -1);
    return 1;
}

static int gpl_getY(lua_State *l) {
    lua_Integer id = luaL_checkinteger(l, 1);
    gff_region_t *reg;
    gff_dude_t *dude = NULL;

    gff_region_manager_get_current(&reg);
    gff_entity_list_for_each(reg->entities, dude) {
        if (dude->ds_id == id) {
            lua_pushnumber(l, dude->mapy - (dude->anim.yoffset + 15) / 16); // the +15 forces a round up.
            return 1;
        }
    }

    lua_pushnumber(l, -1);
    return 1;
}

static int read_complex(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer header = luaL_checkinteger(l, 1);
    lua_Integer depth = luaL_checkinteger(l, 1);

    warn("Need to implement read_complex(" PRI_LI ", " PRI_LI ", " PRI_LI ")\n", obj, header, depth);

    lua_pushinteger(l, 1);
    return 1;
}

static int gpl_rand(lua_State *l) {
    lua_pushinteger(l, rand());
    return 1;
}

static int range(lua_State *l) {
    const uint32_t from = luaL_checkinteger(l, 1);
    const uint32_t to   = luaL_checkinteger(l, 2);

    gff_region_t* reg;
    gff_region_manager_get_current(&reg);
    gff_entity_t* from_entity, *to_entity;
    gff_region_find_entity_by_id(reg, from, &from_entity);
    gff_region_find_entity_by_id(reg, to, &to_entity);

    if (!from_entity || !to_entity) {
        debug("Range from '%u' to '%u' not found returning GPL_RANGE_MAX(%u)\n", from, to, GPL_RANGE_MAX);
        lua_pushinteger(l, GPL_RANGE_MAX);
        goto exit;
    }

    uint32_t xdiff = abs(from_entity->mapx - to_entity->mapx);
    uint32_t ydiff = abs(from_entity->mapy - to_entity->mapy);

    lua_pushinteger(l, xdiff > ydiff ? xdiff : ydiff);
    debug("Range from '%u' to '%u' %u\n", from, to, xdiff > ydiff ? xdiff : ydiff);
exit:
    return 1;
}

static int attack_trigger(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_attack(obj, file, addr);
    return 0;
}

static int attack_trigger_global(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_attack_global(obj, file, addr);
    return 0;
}

static int use_trigger(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_use(obj, file, addr);
    return 0;
}

static int use_trigger_global(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_use_global(obj, file, addr);
    return 0;
}

static int look_trigger(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_look(obj, file, addr);
    return 0;
}

static int look_trigger_global(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_look_global(obj, file, addr);
    return 0;
}

static int noorders_trigger(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);
    lua_Integer trigger = (lua_gettop(l) >= 4) ? luaL_checkinteger(l, 4) : 1;
    lua_Integer run =     (lua_gettop(l) >= 5) ? luaL_checkinteger(l, 5) : 1;

    gff_trigger_add_noorders(obj, file, addr, trigger, run);
    return 0;
}

static int talk_to_trigger(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);

    gff_trigger_add_talkto(obj, file, addr);
    return 0;
}

static int use_with_trigger(lua_State *l) {
    lua_Integer obj1 = luaL_checkinteger(l, 1);
    lua_Integer obj2 = luaL_checkinteger(l, 2);
    lua_Integer file = luaL_checkinteger(l, 3);
    lua_Integer addr = luaL_checkinteger(l, 4);

    gff_trigger_add_usewith(obj1, obj2, file, addr);
    return 0;
}

static int tile_trigger(lua_State *l) {
    lua_Integer x = luaL_checkinteger(l, 1);
    lua_Integer y = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);
    lua_Integer file = luaL_checkinteger(l, 4);
    lua_Integer trip = luaL_checkinteger(l, 5);

    gff_trigger_add_tile(x, y, file, addr, trip);
    return 0;
}

static int box_trigger(lua_State *l) {
    lua_Integer x = luaL_checkinteger(l, 1);
    lua_Integer y = luaL_checkinteger(l, 2);
    lua_Integer w = luaL_checkinteger(l, 3);
    lua_Integer h = luaL_checkinteger(l, 4);
    lua_Integer addr = luaL_checkinteger(l, 5);
    lua_Integer file = luaL_checkinteger(l, 6);
    lua_Integer trip = luaL_checkinteger(l, 7);

    gff_trigger_add_box(x, y, w, h, file, addr, trip);
    return 0;
}

static int request(lua_State *l) {
    lua_Integer cmd = luaL_checkinteger(l, 1);
    const char *obj_type = luaL_checkstring(l, 2);
    lua_Integer num1 = luaL_checkinteger(l, 3);
    lua_Integer num2 = luaL_checkinteger(l, 4);

    //warn("Need to implement: request: cmd: " PRI_LI " obj_type: %s num1: " PRI_LI " num2: " PRI_LI "\n", cmd, obj_type, num1, num2);
    lua_pushinteger(l,
        gff_gpl_request_impl(cmd, atol(obj_type), num1, num2));
    return 1;
}

static int gpl_clone(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer qty = luaL_checkinteger(l, 2);
    lua_Integer x = luaL_checkinteger(l, 3);
    lua_Integer y = luaL_checkinteger(l, 4);
    lua_Integer priority = luaL_checkinteger(l, 5);
    lua_Integer placement = luaL_checkinteger(l, 6);
    int16_t entry_id = -1;
    gff_palette_t *pal = open_files[RESOURCE_GFF_INDEX].pals->palettes;
    gff_region_t *reg;

    debug("gpl-clone: obj: " PRI_LI ", qty: " PRI_LI ", (" PRI_LI ", "
             PRI_LI ") pri: " PRI_LI ", pla:" PRI_LI "\n", obj, qty, x, y,
        priority, placement);

    for (int i = 0; i < qty; i++) {
        debug("Cloning %d to: %lld, %lld\n", obj, x, y);

        gff_dude_t *dude;
        gff_entity_create_from_objex(obj, &dude);
        dude->mapx = x;
        dude->mapy = y;
        dude->anim.xoffset = 0;
        dude->anim.yoffset = 0;
        entry_id = dude->ds_id;
        gff_gpl_set_global(GPL_OTHER, dude);

        if (dude) {
            gff_region_manager_get_current(&reg);
            gff_region_move_to_nearest(reg, dude);
            gff_region_add_entity(reg, dude);
            gff_sprite_load(&(dude->anim), pal, OBJEX_GFF_INDEX, GFF_BMP, dude->anim.bmp_id,
                (dude->name) ? 2 : 1);
            gff_map_place_entity(dude);
            //TODO: Should reshift the entity?
            gff_combat_set_scmd(dude, COMBAT_SCMD_STAND_DOWN);
        }
    }

    lua_pushinteger(l, entry_id);
    return 1;
}

static int gpl_hunt(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    gff_dude_t *dude = NULL;
    gff_region_t *reg;

    gff_region_manager_get_current(&reg);
    gff_entity_list_for_each(reg->entities, dude) {
        if (dude->ds_id == (int)obj) {
            dude->abilities.hunt = 1;
        } 
    }

    return 0;
}

static int gpl_ask_yes_no(lua_State *l) {
    lua_pushinteger(l, gff_narrate_ask_yes_no() == EXIT_SUCCESS ? 1 : 0);
    return 1;
}

static int gpl_tport_party(lua_State *l) {
    lua_Integer map_id = luaL_checkinteger(l, 1);
    lua_Integer mapx = luaL_checkinteger(l, 2);
    lua_Integer mapy = luaL_checkinteger(l, 3);

    error("TPORT PARTY NOT IMPLEMENTED, need to transport to map %lld @ (%lld, %lld).\n", map_id, mapx, mapy);
    gff_window_clear();
    gff_entity_t *player;
    gff_player_get_active(&player);
    player->mapx = mapx;
    player->mapy = mapy;
    gff_window_load_region(map_id);
    //port_entity_update_scmd(player);
    //port_entity_update_scmd(player);
    //gff_center_on_player();

    return 0;
}

static int gpl_tport_everything(lua_State *l) {
    (void)l;
    error("TPORT everything NOT IMPLEMENTED.\n")
    return 0;
}

static int gpl_los_trigger(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);
    lua_Integer param = luaL_checkinteger(l, 4);

    gff_trigger_add_los(obj, file, addr, param);
    return 0;
}

static int gpl_clear_los(lua_State *l) {
    (void)l;
    error("CLEAR LOS NOT IMPLEMENTED\n");
    return 0;
}

static int gpl_fight(lua_State *l) {
    (void)l;
    error("FIGHT NOT IMPLEMENTED\n");
    return 0;
}

static int gpl_wait_on(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    error("WAIT ON (%lld) NOT IMPLEMENTED\n", obj);
    return 0;
}

static int gpl_goxy1(lua_State *l) {
    (void)l;
    lua_Integer x = luaL_checkinteger(l, 1);
    lua_Integer y = luaL_checkinteger(l, 2);
    lua_Integer obj = luaL_checkinteger(l, 3);

    gff_entity_t *dude;
    gff_region_t *reg;

    gff_region_manager_get_current(&reg);
    gff_region_find_entity_by_id(reg, obj, &dude);
    printf("%lld needs to go to (%lld, %lld), currently (%d, %d)\n", obj, x, y, dude->mapx, dude->mapy);
    lua_pushboolean(l, gff_entity_go(dude, x, y) == EXIT_SUCCESS);

    return 1;
}

static int gpl_setthing(lua_State *l) {
    lua_Integer x = luaL_checkinteger(l, 1);
    lua_Integer y = luaL_checkinteger(l, 2);
    (void)x;
    (void)y;
    error("setthing not implemented!\n");

    lua_pushboolean(l, 0);
    return 1;
}

static int gpl_los_trigger_check(lua_State *l) {
    lua_Integer obj = luaL_checkinteger(l, 1);
    lua_Integer file = luaL_checkinteger(l, 2);
    lua_Integer addr = luaL_checkinteger(l, 3);
    lua_Integer param = luaL_checkinteger(l, 4);

    gff_trigger_los_check(obj, file, addr, param);
    return 0;
}

static int call_function(lua_State *l) {
    lua_Integer file = luaL_checkinteger(l, 1);
    lua_Integer addr = luaL_checkinteger(l, 2);

    debug("*****************calling file: " PRI_LI " addr: " PRI_LI "\n", file, addr);
    gff_gpl_lua_execute_script(file, addr, 0);

    return 0;
}

static int gpl_exit(lua_State *l) {
    (void) l;
    gpl_set_exit();
    return 0;
}

static int lua_narrate_open(lua_State *l) {
    lua_Integer cmd = luaL_checkinteger(l, 1);
    const char *text = luaL_checkstring(l, 2);
    lua_Integer index = luaL_checkinteger(l, 3);

    gff_narrate_open(cmd, text, index);
    debug("I need to do " PRI_LI " with text '%s' at index " PRI_LI "\n", cmd, text, index);

    return 0;
}

static int lua_narrate_show(lua_State *l) {
    //lua_pushboolean(l, gff_game_loop_wait_for_signal(WAIT_NARRATE_SELECT));
    gff_game_loop_wait_for_signal(WAIT_NARRATE_SELECT);

    // In exit after a show means we need to exit lua.
    lua_pushboolean(l, gff_gpl_in_exit() == EXIT_SUCCESS);

    return 1;
}

static int lua_play_sound(lua_State *l) {
    lua_Integer bvoc_index = luaL_checkinteger(l, 1);
    debug("I need to play bvoc: " PRI_LI "\n", bvoc_index);
    gff_play_sound_effect(bvoc_index);
    return 0;
}

static int lua_get_active_name(lua_State *l) {
    gff_entity_t *dude;

    gff_player_get_active(&dude);
    lua_pushstring(l, (!dude || !dude->name)
        ? "?"
        : dude->name);

    return 1;
}

static int gpl_set_other_check(lua_State *l) {
    error ("set_other_check not iplemented");
    //lua_Integer ds_id = luaL_checkinteger(l, 1);
    // I believe this checks if other can be set to ds_id (IE: does ds_id exist in region?)
    // If yes, then set other = ds_id and return 1
    // Otherwise return 0;
    //lprintf("if (%s >= 0 and head ~= NULL_OBJECT) or (%s == gpl.id_to_header(%s)) ~= NULL_OBJECT then\n", buf, buf, buf);
    //lua_depth++;
    //lprintf("other1 = %d\n", header);
    //lprintf("accum = 1\n");
    //lua_depth--;
    //lprintf("else\n");
    //lua_depth++;
    //lprintf("accum = 0\n");
    //lua_depth--;
    //lprintf("end\n");
    lua_pushboolean(l, 0);
    return 1;
}

static int lua_debug(lua_State *l) {
    //printf("%s\n", luaL_checkstring(l, 1));
    gpl_lua_debug();
    return 0;
}

static int gpl_error(lua_State *l) {
    //printf("%s\n", luaL_checkstring(l, 1));
    printf("*********************************** ERROR: SUPER ERROR***********************************\n");
    return 0;
}

static const struct luaL_Reg gpl_state_lib[] = {
    {"set_while_callback", set_while_callback},
    {"set_gf", set_gf},
    {"get_gf", get_gf},
    {"set_lf", set_lf},
    {"get_lf", get_lf},
    {"set_gn", set_gn},
    {"get_gn", get_gn},
    {"set_ln", set_ln},
    {"get_ln", get_ln},
    {"set_gbn", set_gbn},
    {"get_gbn", get_gbn},
    {"set_lbn", set_lbn},
    {"get_lbn", get_lbn},
    {"set_gstr", set_gstr},
    {"get_gstr", get_gstr},
    {"get_gname", get_gname},
    {"set_gname", set_gname},
    {"get_pov", get_pov},
    {"get_type", get_type},
    {"get_id", get_id},
    {"get_element", get_element},
    {"set_element", set_element},
    {"get_party", get_party},
    {"is_true", is_true},
    {"getX", gpl_getX},
    {"getY", gpl_getY},
    {"read_complex", read_complex},
    {"rand", gpl_rand},
    {"range", range},
    {"attack_trigger", attack_trigger},
    {"attack_trigger_global", attack_trigger_global},
    {"noorders_trigger", noorders_trigger},
    {"use_trigger", use_trigger},
    {"use_trigger_global", use_trigger_global},
    {"look_trigger", look_trigger},
    {"look_trigger_global", look_trigger_global},
    {"talk_to_trigger", talk_to_trigger},
    {"use_with_trigger", use_with_trigger},
    {"tile_trigger", tile_trigger},
    {"box_trigger", box_trigger},
    {"call_function", call_function},
    {"request", request},
    {"clone", gpl_clone},
    {"hunt", gpl_hunt},
    {"ask_yes_no", gpl_ask_yes_no},
    {"tport_party", gpl_tport_party},
    {"tport_everything", gpl_tport_everything},
    {"los_trigger", gpl_los_trigger},
    {"los_trigger_check", gpl_los_trigger_check},
    {"clear_los", gpl_clear_los},
    {"fight", gpl_fight},
    {"wait_on", gpl_wait_on},
    {"goxy1", gpl_goxy1},
    {"setthing", gpl_setthing},
    {"exit", gpl_exit},
    {"set_other_check", gpl_set_other_check},
    {"narrate_open", lua_narrate_open},
    {"narrate_show", lua_narrate_show},
    {"play_sound", lua_play_sound},
    {"get_active_name", lua_get_active_name},
    {"debug", lua_debug},
    {"error", gpl_error},
    {NULL, NULL}
} ;

static void set_globals(lua_State *l) {
    char buf[BUF_SIZE];
    snprintf(buf, BUF_SIZE, "NAR_ADD_MENU = %d\n", NAR_ADD_MENU);
    luaL_dostring(l, buf);
    snprintf(buf, BUF_SIZE, "NAR_PORTRAIT = %d\n", NAR_PORTRAIT);
    luaL_dostring(l, buf);
    snprintf(buf, BUF_SIZE, "NAR_SHOW_TEXT = %d\n", NAR_SHOW_TEXT);
    luaL_dostring(l, buf);
    snprintf(buf, BUF_SIZE, "NAR_SHOW_MENU = %d\n", NAR_SHOW_MENU);
    luaL_dostring(l, buf);
    snprintf(buf, BUF_SIZE, "NAR_EDIT_BOX = %d\n", NAR_EDIT_BOX);
    luaL_dostring(l, buf);
}

extern int gff_gpl_state_register(lua_State *l) {
    set_globals(l);
    lua_newtable(l);
    luaL_setfuncs(l, gpl_state_lib, 0);
    lua_setglobal(l, "gpl");
    return EXIT_SUCCESS;
}
*/

extern int gff_gpl_local_clear() {
    memset(gpl_local_flags, 0x0, sizeof(int8_t) * MAX_LFLAGS);
    memset(gpl_local_nums, 0x0, sizeof(int16_t) * MAX_LNUMS);
    memset(gpl_local_bnums, 0x0, sizeof(int32_t) * MAX_LBIGNUMS);
    //memset(gpl_local_bnums, 0x0, sizeof(int32_t) * MAX_LBIGNUMS); string!
    return EXIT_SUCCESS;
}

extern int gff_gpl_serialize_globals(uint32_t *len, char **serial) {
    if (!len || !serial) { return EXIT_FAILURE; }
    *len =
        sizeof(gpl_global_flags) +
        sizeof(gpl_global_nums) +
        sizeof(gpl_global_bnums) +
        sizeof(gpl_global_strs) +
        sizeof(gpl_gnames);
    char *ret = malloc(*len);
    if (!ret) { return EXIT_FAILURE; }
    char *buf = ret;
    memcpy(buf, gpl_global_flags, sizeof(gpl_global_flags));
    buf += sizeof(gpl_global_flags);
    memcpy(buf, gpl_global_nums, sizeof(gpl_global_nums));
    buf += sizeof(gpl_global_nums);
    memcpy(buf, gpl_global_bnums, sizeof(gpl_global_bnums));
    buf += sizeof(gpl_global_bnums);
    memcpy(buf, gpl_global_strs, sizeof(gpl_global_strs));
    buf += sizeof(gpl_global_strs);
    memcpy(buf, gpl_gnames, sizeof(gpl_gnames));
    buf += sizeof(gpl_gnames);

    *serial = ret;
    return EXIT_SUCCESS;
}

extern int gff_gpl_deserialize_globals(char *buf) {
    memcpy(gpl_global_flags, buf, sizeof(gpl_global_flags));
    buf += sizeof(gpl_global_flags);
    memcpy(gpl_global_nums, buf, sizeof(gpl_global_nums));
    buf += sizeof(gpl_global_nums);
    memcpy(gpl_global_bnums, buf, sizeof(gpl_global_bnums));
    buf += sizeof(gpl_global_bnums);
    memcpy(gpl_global_strs, buf, sizeof(gpl_global_strs));
    buf += sizeof(gpl_global_strs);
    memcpy(gpl_gnames, buf, sizeof(gpl_gnames));
    buf += sizeof(gpl_gnames);
    return EXIT_SUCCESS;
}

extern int gff_gpl_deserialize_locals(char *buf) {
    memcpy(gpl_local_flags, buf, sizeof(gpl_local_flags));
    buf += sizeof(gpl_local_flags);
    memcpy(gpl_local_nums, buf, sizeof(gpl_local_nums));
    buf += sizeof(gpl_local_nums);
    memcpy(gpl_local_bnums, buf, sizeof(gpl_local_bnums));
    buf += sizeof(gpl_local_bnums);
    return EXIT_SUCCESS;
}

extern int gff_gpl_serialize_locals(uint32_t *len, char **serial) {
    if (!len || !serial) { return EXIT_FAILURE; }
    *len =
        sizeof(gpl_local_flags) +
        sizeof(gpl_local_nums) +
        sizeof(gpl_local_bnums);
    char *ret = malloc(*len);
    if (!ret) { return EXIT_FAILURE; }
    char *buf = ret;
    memcpy(buf, gpl_local_flags, sizeof(gpl_local_flags));
    buf += sizeof(gpl_local_flags);
    memcpy(buf, gpl_local_nums, sizeof(gpl_local_nums));
    buf += sizeof(gpl_local_nums);
    memcpy(buf, gpl_local_bnums, sizeof(gpl_local_bnums));
    buf += sizeof(gpl_local_bnums);

    *serial = ret;
    return EXIT_SUCCESS;
}

extern int gff_gpl_get_gname(gpl_gnum_t pos, int16_t *gn) {
    if (pos < 0 || pos >= MAX_GNAMES) { return EXIT_FAILURE; }
    //printf("gpl_get_gname[%d] = %d\n", pos, gpl_gnames[pos]);
    *gn = gpl_gnames[pos];
    return EXIT_SUCCESS;
}
