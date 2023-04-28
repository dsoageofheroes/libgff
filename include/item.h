#ifndef GFF_ITEM_H
#define GFF_ITEM_H

#include <stdint.h>

#include "common.h"
#include "status.h"

#define DS1_WEAPON_MELEE     (1<<0)
#define DS1_WEAPON_MISSILE   (1<<1)
#define DS1_WEAPON_SHIELD    (1<<2)
#define DS1_WEAPON_USE_AMMO  (1<<3)
#define DS1_WEAPON_THROWN    (1<<4)
#define DS1_ARMOR_FLAG       (1<<7)

enum gff_door_status_e {
    GFF_DOOR_OPEN   = 0x00,
    GFF_DOOR_CLOSED = 0x01,
    GFF_DOOR_LOCKED = 0x02,
    GFF_DOOR_GPL    = 0x04,
    GFF_DOOR_SECRET = 0x08,
};

// First the DS1 items structs
typedef struct ds1_item_s { // Not confirmed at all...
    int16_t  id; // 0, confirmed (but is negative...), is the OJFF entry
    uint16_t quantity; // confirmed, 0 mean no need.
    int16_t  next;  // 4, for some internal book keeping.
    uint16_t value; // 6, confirmed
    int16_t  pack_index;
    int16_t  item_index; // Correct, maps into it1r.
    int16_t  icon;
    uint16_t charges;
    uint8_t  special;  // confirmed
    uint8_t  slot;     // confirmed
    uint8_t  name_idx; // confirmed
    int8_t   bonus;
    uint16_t priority;
    int8_t   data0;
} __attribute__ ((__packed__)) ds1_item_t;

typedef struct ds_item1r_s {
    uint8_t weapon_type;
    uint8_t data0; // always 0, probably structure alignment byte.
    uint16_t damage_type;
    uint8_t weight;
    uint16_t data1;
    uint8_t base_hp;
    uint8_t material;
    uint8_t placement;
    uint8_t range;// Need to confirm
    uint8_t num_attacks;
    uint8_t sides;
    uint8_t dice;
    int8_t mod;
    uint8_t flags;
    uint16_t legal_class;
    int8_t base_AC;
    uint8_t data2; // padding?
} __attribute__ ((__packed__)) ds_item1r_t;

//End of DS1 item structs

enum {
    SLOT_ARM, SLOT_AMMO, SLOT_MISSILE, SLOT_HAND0, SLOT_FINGER0, SLOT_WAIST,
    SLOT_LEGS, SLOT_HEAD, SLOT_NECK, SLOT_CHEST, SLOT_HAND1, SLOT_FINGER1,
    SLOT_CLOAK, SLOT_FOOT,
    SLOT_END,
    // These are for combat
    SLOT_INNATE0,
    SLOT_INNATE1,
    SLOT_INNATE2,
};

typedef struct gff_ds_inventory_s {
    ds1_item_t arm;
    ds1_item_t ammo;
    ds1_item_t missile;
    ds1_item_t hand0;
    ds1_item_t finger0;
    ds1_item_t waist;
    ds1_item_t legs;
    ds1_item_t head;
    ds1_item_t neck;
    ds1_item_t chest;
    ds1_item_t hand1;
    ds1_item_t finger1;
    ds1_item_t cloak;
    ds1_item_t foot;
    ds1_item_t bp[12];
} gff_ds1_inventory_t;

/*
extern void               gff_item_init();
extern int                gff_item_load(ds1_item_t *item, int32_t id);
extern const char        *gff_item_name(const int32_t name_idx);
extern const ds_item1r_t *gff_get_item1r(const int32_t item_idx);
extern int32_t            gff_item_get_bmp_id(ds1_item_t *item);
extern void               gff_item_close();
extern int                gff_item_allowed_in_slot(ds1_item_t *item, const int slot);
*/

extern int gff_item_read(gff_file_t *f, int id, ds1_item_t *item);

#endif
