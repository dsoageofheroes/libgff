#include <gff/gff.h>
#include <gff/gfftypes.h>
#include <gff/image.h>
#include <gff/item.h>
#include <gff/manager.h>
#include <gff/region.h>
#include <gff/gui.h>
#include <gpl/gpl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

extern gff_manager_t *man; // forward from gffmod.c
static int indent = 0;

#define printfi(...) { printf("%*s", indent, ""); printf(__VA_ARGS__); }

static void print_ojff(gff_file_t *gff, uint32_t res_id);
static int print_combat(ds1_combat_t *dc, const char *prefix);
static int print_item(ds1_item_t *item, const char *prefix);

static void print_gff_frame(gff_frame_t *frame) {
    printf("    flags: %d, ", frame->flags);
    printf("type: %d, ", frame->type);
    printf("width: %d, ", frame->width);
    printf("height: %d\n", frame->height);
    printf("    bounds: { (%d, %d) -> (%d, %d) }\n",
            frame->bounds.xmin,
            frame->bounds.ymin,
            frame->bounds.xmax,
            frame->bounds.xmax);
    printf("    initbounds: { (%d, %d) -> (%d, %d) }\n",
            frame->initbounds.xmin,
            frame->initbounds.ymin,
            frame->initbounds.xmax,
            frame->initbounds.xmax);
    printf("    zonebounds: { (%d, %d) -> (%d, %d) }\n",
            frame->zonebounds.xmin,
            frame->zonebounds.ymin,
            frame->zonebounds.xmax,
            frame->zonebounds.xmax);
    printf("    border_bmp: %d\n", frame->border_bmp);
    printf("    background_bmp: %d\n", frame->background_bmp);
    printf("    title: %s\n", frame->title);
}

static void print_button_item(gff_file_t *gff, unsigned int id, gff_gui_item_t *item) {
    gff_button_t button;

    gff_read_button(gff, id, &button);
    if (item) {
        button.frame.initbounds = item->init_bounds;
    }
    printfi("BUTTON #%d\n", id);
    printfi("    header = {type: %d, len: %d, id: %d}\n", button.rh.type, button.rh.len, button.rh.id);
    printfi("    icon: %d @ (%d, %d)\n", button.icon_id, button.iconx, button.icony);
    indent += 4;
    print_gff_frame(&button.frame);
    indent -= 4;
    printfi("    flags: %d\n", button.flags);
    printfi("    userid: %d\n", button.userid);
    printfi("    text: '%s' (%d, %d)\n", button.text, button.textx, button.texty);
    printfi("    key: %d\n", button.key);
}

static void print_button(gff_file_t *gff, unsigned int id) {
    print_button_item(gff, id, NULL);
}


static void print_gffi(gff_file_t *gff, unsigned int id) {
    gff_chunk_entry_t *gffi = gff->chunks[id];

    printf("GFFI[%d]:\n", id);
    if (gffi->chunk_count & GFFSEGFLAGMASK) {
        printf("    seg:    num_entries: %u  seg_count: %u location: %u\n", 
                gffi->segs.header.num_entries,
                gffi->segs.header.seg_count,
                gffi->segs.header.seg_loc_id
              );
        for (int i = 0; i < gffi->segs.header.num_entries; i++) {
            printf("        %d: first_id:%d num_chunks: %d\n",
                    i,
                    gffi->segs.segs[i].first_id,
                    gffi->segs.segs[i].num_chunks);
        }
    } else {
        printf("    chunks: total: %lu\n", gffi->chunk_count & GFFMAXCHUNKMASK);
        uint32_t max = gffi->chunk_count & GFFMAXCHUNKMASK;
        for (int i = 0; i < max; i++) {
            printf("        %d: id: %u loc: %u length: %u\n", i,
                    gffi->chunks[i].id,
                    gffi->chunks[i].location,
                    gffi->chunks[i].length);
        }
    }
}

static void print_text(gff_file_t *gff, unsigned int id) {
    char text[4096];

    if (gff_read_text(gff, id, text, 4096)) {
        printf("Unable to read TEXT @ %d\n", id);
    }

    printf("Text[%d]: '%s'\n", id, text);
}

static void print_etme(gff_file_t *gff, unsigned int id) {
    char text[4096];

    if (gff_read_etme(gff, id, text, 4096)) {
        printf("Unable to read ETME @ %d\n", id);
    }

    printf("ETME[%d]:\n'%s'\n", id, text);
}

static void print_name(gff_file_t *gff, unsigned int id) {
    char text[16384];
    uint32_t len;

    if (gff_read_names(gff, id, text, 16384, &len)) {
        printf("Unable to read NAME @ %d\n", id);
    }

    printf("Name #%d: count: %d\n", id, len);

    for (int i = 0; i < len; i++) {
        printf("    %d: '%s'\n", i, text + (25 * i));
    }
}

static void print_item1r(ds_item1r_t *item1r, const char *prefix) {
    printf("%s    weapon: {type: %d, damage_type: %d, range: %d}\n",
            prefix,
            item1r->weapon_type,
            item1r->damage_type,
            item1r->range);
    printf("%s    weight: %d\n", prefix, item1r->weight);
    printf("%s    base_hp: %d\n", prefix, item1r->base_hp);
    printf("%s    material: %d\n", prefix, item1r->material);
    printf("%s    placement: %d\n", prefix, item1r->placement);
    printf("%s    damage: %d x %d D %d\n",
            prefix,
            item1r->num_attacks,
            item1r->dice,
            item1r->sides);
    printf("%s    mod: %d\n", prefix, item1r->mod);
    printf("%s    flags: %d\n", prefix, item1r->flags);
    printf("%s    class: %d\n", prefix, item1r->legal_class);
    printf("%s    AC: %d\n", prefix, item1r->base_AC);
}

static void print_it1r(gff_file_t *gff, unsigned int id) {
    ds_item1r_t *item1rs = NULL;
    uint32_t     num_item1rs = 0;
    char         names[16384];
    uint32_t     len;

    if (gff_read_names(gff, id, names, 16384, &len)) {
        printf("Unable to read NAME @ %d\n", id);
    }

    if (gff_read_it1r(gff, id, &item1rs, &num_item1rs)) {
        printf("Unable to return IT1R #%d\n", id);
        return;
    }

    for (int i = 0; i < num_item1rs; i++) {
        printf("IT1R #%d: %s\n", i, names + 25 * i);
        print_item1r(item1rs + i, "");
    }

    free(item1rs);
}

static void print_spin(gff_file_t *gff, unsigned int id) {
    char spin[4096];

    if (gff_read_spin(gff, id, spin, 4096)) {
        printf("Unable to read SPIN @ %d\n", id);
    }

    printf("SPIN[%d]: '%s'\n", id, spin);
}

static void print_rdat(gff_file_t *gff, unsigned int id) {
    char rdat[4096];

    if (gff_read_spin(gff, id, rdat, 4096)) {
        printf("Unable to read RDAT @ %d\n", id);
    }

    printf("RDAT[%d]: '%s'\n", id, rdat);
}

static void print_monr(gff_file_t *gff, unsigned int id) {
    gff_monster_list_t *mon = NULL;
    size_t num_monsters, num_regions, size;

    gff_load_monster_list(gff, id, &mon, &size);

    num_regions = size / sizeof(gff_monster_list_t);

    printf("monster list #%d has %d regions.\n", id, num_regions);

    for (int i = 0; i < num_regions; i++) {
        printf("    Region %d:\n", mon[i].region);
        for (int j = 0; j < MAX_MONSTERS_PER_REGION; j++) {
            printf("        id: %d, level: %d\n", mon[i].monsters[j].id, mon[i].monsters[j].level);
        }
        //printf("    %d: id: %d level: %d\n", i, mon->monsters[i].id, mon->monsters[i].level);
    }

    free(mon);
}

static void print_font(gff_file_t *gff, unsigned int id) {
    gff_font_t *font = NULL;

    if (gff_load_font(gff, id, &font)) {
        printf("Unable to read FONT\n");
        return;
    }

    printf("font %d:\n", id);
    printf("    num: %d\n", font->num);
    printf("    height: %d\n", font->height);
    printf("    background_color: %d\n", font->background_color);
    printf("    flags: 0x%x\n", font->flags);
    //uint8_t  colors[256];
    //uint16_t char_offset[];
    printf("Need to complete font...\n");

    free(font);
}

static void print_merr(gff_file_t *gff, unsigned int id) {
    char merr[4096];

    if (gff_read_merr(gff, id, merr, 4096)) {
        printf("Unable to read MERR @ %d\n", id);
    }

    printf("MERR[%d]: '%s'\n", id, merr);
}

static void print_cseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_CSEQ, id)) {
        printf("unable to read CSEQ #%d\n", id);
        return;
    }
    printf("CSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_gseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_GSEQ, id)) {
        printf("unable to read GSEQ #%d\n", id);
        return;
    }
    printf("GSEQ #%d: length is %d, ", id, chunk.length);

    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_pseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_PSEQ, id)) {
        printf("unable to read PSEQ #%d\n", id);
        return;
    }
    printf("PSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_psin(gff_file_t *gff, unsigned int id) {
    gff_psin_t psin;

    if (gff_read_psin(gff, id, &psin)) {
        printf("Unable to read PSIN @ %d\n", id);
    }

    printf("PSIN[%d]: ", id);
    for (int i = 0; i < 6; i++) {
        if (psin.types[i]) {
            printf("%s ", gff_get_psin_name(i));
        }
    }
    if (psin.types[6] == 0xff) {
        printf("ALL-PSIONICS" );
    }
    printf("\n");
}

static void print_psst(gff_file_t *gff, unsigned int id) {
    gff_psst_t psst;
    char psin_name[1024];

    if (gff_read_psst(gff, id, &psst)) {
        printf("Unable to read PSST @ %d\n", id);
    }

    printf("PSST[%d]: ", id);
    for (int i = 0; i < sizeof(gff_psst_t); i++) {
        if (psst.psionics[i]) {
            if (man->ds1.resource) {
                gff_read_spin(man->ds1.resource, 139 + i, psin_name, 1024);
                printf("\n    %s", psin_name);
            } else {
                printf(" %d", i);
            }
        }
    }

    if (!man->ds1.resource) {
        printf(" specify ds1 with -1 to get full names");
    }
    printf("\n");
}

static void print_spst(gff_file_t *gff, unsigned int id) {
    gff_spst_t psst;
    char spell_name[1024];

    if (gff_read_spst(gff, id, &psst)) {
        printf("Unable to read PSST @ %d\n", id);
    }

    //printf("MAX_SPELLS = %d\n", MAX_SPELLS);
    //printf("sizeof(SPST) = %lu\n", sizeof(gff_spst_t));
    printf("SPST[%d]: \n", id);
    for (int p = 0; p < 4; p++) {
        printf("  player %d:", p);
    for (int i = 0; i < MAX_WIZARD_SPELL; i++) {
        if (gff_has_spell(&psst, p, i)) {
            if (man->ds1.resource) {
                gff_read_spin(man->ds1.resource, i, spell_name, 1024);
                printf("\n    %s", spell_name);
            } else {
                printf(" %d", i);
            }
        }
    }
        printf("\n");
    }

    if (!man->ds1.resource) {
        printf(" specify ds1 with -1 to get full names");
    }
    printf("\n");
}

static void print_lseq(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;

    if (gff_find_chunk_header(gff, &chunk, GFF_LSEQ, id)) {
        printf("unable to read LSEQ #%d\n", id);
        return;
    }
    printf("LSEQ #%d: length is %d, ", id, chunk.length);
    printf("Use `-x <dir>` to extract all xmis to a directory.\n");
}

static void print_bvoc(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;
    uint8_t *data = NULL;

    if (gff_find_chunk_header(gff, &chunk, GFF_BVOC, id)) {
        printf("unable to read BVOC #%d\n", id);
        return;
    }
    printf("BVOC #%d: length is %d, \n", id, chunk.length);

    data = malloc(chunk.length);
    if (gff_read_chunk(gff, &chunk, data, chunk.length) != chunk.length) {
        printf("Can't read BVOC #%d\n", id);
        return;
    }
    free(data);
}

static void print_accl(gff_file_t *gff, unsigned int id) {
    gff_chunk_header_t chunk;
    uint8_t *data = NULL;
    gff_accl_t *accl;

    if (gff_find_chunk_header(gff, &chunk, GFF_ACCL, id)) {
        printfi("unable to read ACCL #%d\n", id);
        return;
    }
    printfi("ACCL #%d: length is %d\n ", id, chunk.length);

    data = malloc(chunk.length);
    if (gff_read_chunk(gff, &chunk, data, chunk.length) != chunk.length) {
        printfi("Can't read ACCL #%d\n", id);
        return;
    }

    accl = (gff_accl_t*)data;
    for (int i = 0; i < accl->count; i++) {
        printfi("    %d: flags: 0x%x event: %d user_id: %d\n", i, accl->entries[i].flags, accl->entries[i].event, accl->entries[i].user_id);
    }

    free(data);
}

static void print_ebox(gff_file_t *gff, unsigned int id) {
    gff_ebox_t ebox;
    //gff_frame_t frame;

    if (gff_read_ebox(gff, id, &ebox)) {
        printfi("unable to read EBOX #%d\n", id);
        return;
    }

    printfi("EBOX #%d:\n", id);
    printfi("    header = {type: %d, len: %d, id: %d}\n", ebox.rh.type, ebox.rh.len, ebox.rh.id);
    printfi("    max_lines: %d, ", ebox.max_lines);
    printfi("styles: %d, ", ebox.styles);
    printfi("runs: %d, ", ebox.runs);
    printfi("size: %d\n", ebox.size);
    printfi("    user_id: %d\n", ebox.user_id);
    printfi("    bounds: { (%d, %d) -> (%d, %d) }\n",
            ebox.frame.bounds.xmin,
            ebox.frame.bounds.ymin,
            ebox.frame.bounds.xmax,
            ebox.frame.bounds.xmax);
    printfi("    initbounds: { (%d, %d) -> (%d, %d) }\n",
            ebox.frame.initbounds.xmin,
            ebox.frame.initbounds.ymin,
            ebox.frame.initbounds.xmax,
            ebox.frame.initbounds.xmax);
    printfi("    zonebounds: { (%d, %d) -> (%d, %d) }\n",
            ebox.frame.zonebounds.xmin,
            ebox.frame.zonebounds.ymin,
            ebox.frame.zonebounds.xmax,
            ebox.frame.zonebounds.xmax);
    printfi("    border_bmp: %d\n", ebox.frame.border_bmp);
    printfi("    background_bmp: %d\n", ebox.frame.background_bmp);
}

static void print_frame(gff_file_t *gff, unsigned int id) {
    gff_full_frame_t frame;

    if (gff_read_frame(gff, id, &frame)) {
        printfi("unable to read Frame #%d\n", id);
        return;
    }

    printfi("APFM #%d:\n", id);
    printfi("    header = {type: %d, len: %d, id: %d}\n", frame.rh.type, frame.rh.len, frame.rh.id);
    print_gff_frame(&frame.frame);
    printfi("    event_filter: %d\n", frame.event_filter);
    printfi("    snap: { mode: %d, (%x, %x), w: %d, h: %d }\n",
            frame.snap_mode,
            frame.snap_x,
            frame.snap_y,
            frame.snap_width,
            frame.snap_height);
}

static void print_window(gff_file_t *gff, unsigned int id) {
    gff_window_t *win = NULL;
    uint8_t      *buf = NULL;

    gff_load_window(gff, id, &win);
    printfi("WINDOW #%d:\n", id);
    printfi("    header = {type: %d, len: %d, id: %d}\n", win->rh.type, win->rh.len, win->rh.id);
    /*
    printf("    region:\n");
    printf("        depth: %d\n", win->region.depth);
    printf("        bounds: { (%d, %d) -> (%d, %d)\n",
            win->region.bounds.xmin,
            win->region.bounds.ymin,
            win->region.bounds.xmax,
            win->region.bounds.ymax);
    for (int i = 0; i < 16; i++) {
        printf("        region[%d]: { (%d, %d) -> (%d, %d)\n", i,
                win->region.regions[i].xmin,
                win->region.regions[i].ymin,
                win->region.regions[i].xmax,
                win->region.regions[i].ymax);
    }
    */
    printfi("    pos: (%d, %d)\n", win->x, win->y);
    printfi("    flags: %d\n", win->flags);
    print_gff_frame(&win->frame);
    printfi("    offset: (%d, %d)\n", win->offsetx, win->offsety);
    printfi("    item_count: %d\n", win->itemCount);

    buf = (uint8_t*)win;
    indent += 4;
    for (int i = 0; i < win->itemCount; i++) {
        gff_gui_item_t *item = (gff_gui_item_t*)(buf + sizeof(gff_window_t) + 12 + i *(sizeof(gff_gui_item_t)));
        switch (item->type) {
            case GFF_ACCL:
                print_accl(gff, item->id);
                break;
            case GFF_APFM:
                print_frame(gff, item->id);
                break;
            case GFF_BUTN:
                print_button_item(gff, item->id, item);
                break;
            case GFF_EBOX:
                print_ebox(gff, item->id);
                break;
            default:
                printf("UNKNOWN TYPE IN WINDOW: %d\n", item->type);
        }
    }
    indent -= 4;

    free(win);
}

static void print_icon(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_ICON, id);


    printf("ICON #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_ICON, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_port(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_PORT, id);


    printf("PORT #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_PORT, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_wall(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_WALL, id);


    printf("WALL #%d:\n", id);
    printf("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_WALL, id, i, &info);
        printf("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_etab(gff_file_t *gff, uint32_t id) {
    gff_etab_object_t *etabs;
    uint32_t amt;

    if (gff_load_etab(gff, id, &etabs, &amt)) {
        printf("Can't read ETAB.\n");
        return;
    }

    for (int i = 0; i < amt; i++) {
        gff_etab_object_t *etab = etabs + i;
        printf("ETAB #%d:\n", i);
        printf("    pos: { %d, %d, %d }\n",
                etab->xpos,
                etab->ypos,
                etab->zpos);
        printf("    flags: %d\n", etab->flags);
        if (man->ds1.segobjex) {
            indent += 4;
            print_ojff(man->ds1.segobjex, -1 * etab->index);
            indent -= 4;
        } else {
            printf("    index: %d specify -1 to get more information.\n", etab->index);
        }
    }

    free(etabs);
}

static void print_char(gff_file_t *gff, unsigned int id) {
    uint32_t len = 0, pos = 0;
    gff_char_entry_t *gchar;
    gff_rdff_header_t *rdff;
    void *data = NULL;

    if (gff_load_char(gff, id, &gchar, &len)) {
        printf("Can't read CHAR.\n");
        return;
    }

    printf("action = %d(%s),", gchar->header.load_action, "");//gchar->header[gchar->header.load_action]);
    printf("blocknum = %d,", gchar->header.blocknum);
    printf("type = %d(%s),", gchar->header.type, "");//gchar->header[gchar->header.type]);
    printf("index = %d,", gchar->header.index);
    printf("from = %d,", gchar->header.from);
    printf("len = %d\n", gchar->header.len);
    ds1_item_t *item = NULL;
    while (pos < len) {
        printf("pos: %u, len: %u\n", pos, len);
        rdff = (gff_rdff_header_t*)(((char*)gchar) + pos);
        data = rdff + 1;
        pos += sizeof(gff_rdff_header_t);
        switch(rdff->load_action) {
            case RDFF_OBJECT:
                switch(rdff->type) {
                    case GFF_COMBAT_OBJECT:
                        printf("sizeof: %lu\n", sizeof(ds1_combat_t));
                        print_combat((ds1_combat_t*)data, "        ");
                        break;
                    case GFF_ITEM_OBJECT:
                        printf("ITEM\n");
                        break;
                    case GFF_CHAR_OBJECT:
                        printf("CHAR OBJECT\n");
                    case GFF_MINI_OBJECT:
                        printf("MINI\n");
                    default:
                        printf("Unexpected RDFF_OBJECT for char.\n");
                        break;
                }
                break;
            case RDFF_CONTAINER:
                printf("container: with len %u\n", rdff->len);
                item = (ds1_item_t*)(((char*)rdff) + sizeof(gff_rdff_header_t));
                print_item(item, "    ");
                break;
            case RDFF_DATA:
                printf("data ");
                switch(rdff->type) {
                    case RDFF_DATA_COMBAT:
                        printf("?Combat\n");
                        break;
                    case RDFF_DATA_ITEM1R:
                        printf("item1r (This is ignore during ds1 character loading)\n");
                        ds_item1r_t *item1r = (ds_item1r_t*)(((char*)rdff) + sizeof(gff_rdff_header_t));
                        print_item1r(item1r, "    ");
                        //print_item(item, "    ");
                        break;
                    case RDFF_DATA_ITEM:
                        printf("ITEM (This is ignore during character loading)\n");
                        item = (ds1_item_t*)(((char*)rdff) + sizeof(gff_rdff_header_t));
                        print_item(item, "    ");
                        break;
                    case RDFF_DATA_CHARREC:
                    case RDFF_DATA_MINI:
                    case RDFF_DATA_NAMEIX:
                    default:
                        printf(" UNKNOWN(%d)\n", rdff->type);
                        break;
                }
                break;
            case RDFF_NEXT:
                item = (ds1_item_t*)(((char*)rdff) + sizeof(gff_rdff_header_t));
                print_item(item, "    ");
                break;
            case RDFF_END:
                printf("end\n");
                break;
            default:
                printf("Unknown load action %d\n", rdff->load_action);
        }
        pos += rdff->len;
    }

    free(gchar);
}


static void print_bmp(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_BMP, id);

    printfi("BMP #%d:\n", id);
    printfi("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_BMP, id, i, &info);
        printfi("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_tile(gff_file_t *gff, uint32_t id) {
    gff_frame_info_t info;
    int fc = gff_get_frame_count(gff, GFF_TILE, id);

    printfi("TILE #%d:\n", id);
    printfi("    frames: %d\n", fc);
    for (int i = 0; i < fc; i++) {
        gff_frame_info(gff, GFF_TILE, id, i, &info);
        printfi("    frame %d: %d x %d\n", i, info.w, info.h);
    }
}

static void print_gmap(gff_file_t *gff, uint32_t id) {
    uint8_t flags[MAP_ROWS][MAP_COLUMNS];

    if (gff_read_global_flags(gff, id, flags)) {
        printf("Unable to read GMAP.\n");
        return;
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLUMNS; j++) {
            printf("%d ", flags[i][j]);
        }
        printf("\n");
    }
}

static void print_rmap(gff_file_t *gff, uint32_t id) {
    uint8_t flags[MAP_ROWS][MAP_COLUMNS];

    if (gff_read_region_flags(gff, id, flags)) {
        printf("Unable to read GMAP.\n");
        return;
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLUMNS; j++) {
            printf("%d ", flags[i][j]);
        }
        printf("\n");
    }
}

static void print_rdff(gff_file_t *gff, uint32_t id) {
    gff_rdff_t rdff;

    if (gff_read_rdff(gff, id, &rdff)) {
        printf("Unable to load RDFF.\n");
        return;
    }

    switch(rdff.header.load_action) {
        case RDFF_OBJECT:
            printf("RDFF #%d (OBJECT): ", id);
            switch(rdff.header.type) {
                case GFF_COMBAT_OBJECT:
                    printf("Combat\n");
                    break;
                case GFF_ITEM_OBJECT:
                    printf("ITEM\n");
                    break;
                case GFF_CHAR_OBJECT:
                    printf("CHAR\n");
                    break;
                case GFF_MINI_OBJECT:
                    printf("MINI\n");
                    break;
                default:
                    printf("Unknown\n");
                    break;
            }
            break;
        case RDFF_CONTAINER:
            printf("RDFF #%d (CONTAINER):\n", id);
            break;
        case RDFF_DATA:
            printf("RDFF #%d (DATA):\n", id);
            break;
        case RDFF_NEXT:
            printf("RDFF #%d (NEXT):\n", id);
            break;
        case RDFF_END:
            printf("RDFF #%d (END):\n", id);
            break;
    }
}

static void print_scmd(gff_file_t *gff, uint32_t res_id) {
    gff_scmd_t *scmd = NULL;
    uint32_t len;
    int last = 0;

    if (gff_scmd_read(gff, res_id, &scmd, &len)) {
        printf("Unable to read SCMD.\n");
        return;
    }

    printfi("SCMD #%d\n", res_id);
    for (int i = 0; !last && i < len; i++ ) {
        printfi("    [%d]: bmp: %d, delay: %d, flags %d, %d x %d, offset: %d x %d, sound: %d\n",
                i,
                scmd[i].bmp_idx,
                scmd[i].delay,
                scmd[i].flags,
                scmd[i].xoffset,
                scmd[i].yoffset,
                scmd[i].xoffsethot,
                scmd[i].yoffsethot,
                scmd[i].soundidx);
        last = scmd[i].flags & (SCMD_LAST | SCMD_JUMP);
    }

    free(scmd);
}

static void print_ojff(gff_file_t *gff, uint32_t res_id) {
    gff_ojff_t ojff;

    if (gff_ojff_read(gff, res_id, &ojff)) {
        printfi("Can't read OJFF\n");
        return;
    }

    printfi("OJFF: #%d\n", res_id);
    printfi("    flags: %d\n", ojff.flags);
    printfi("    offset: %d x %d\n", ojff.xoffset, ojff.yoffset);
    printfi("    pos: %d x %d x %d\n", ojff.xpos, ojff.ypos, ojff.zpos);
    printfi("    obj: %d\n", ojff.object_index);
    printfi("    bmp: %d\n", ojff.bmp_id);
    if (ojff.bmp_id > 0) {
        indent += 4;
        print_bmp(gff, ojff.bmp_id);
        indent -= 4;
    }
    printfi("    script: %d\n", ojff.script_id);
    if (ojff.script_id > 0) {
        indent += 4;
        print_scmd(gff, ojff.script_id);
        indent -= 4;
    }
}

static void print_mas(gff_file_t *gff, unsigned int id) {
    uint8_t *mas = NULL;
    size_t len;

    if (gff_load_mas(gff, id, &mas, &len)) {
        printf("unable to load MAS\n");
        return;
    }

    printf("MAS #%d: length: %zu\n", id, len);

    gff_gpl_parse(mas, len, NULL, 1);

    free(mas);
}

static void print_gpl(gff_file_t *gff, unsigned int id) {
    uint8_t *gpl = NULL;
    size_t len = 0;

    if (gff_load_gpl(gff, id, &gpl, &len)) {
        printf("unable to load GPL\n");
        return;
    }

    printf("GPL #%d: length: %zu\n", id, len);

    gff_gpl_parse(gpl, len, NULL, 0);

    free(gpl);
}

static void print_gplx(gff_file_t *gff, unsigned int id) {
    uint8_t *gpl = NULL;
    size_t len;

    if (gff_load_gplx(gff, id, &gpl, &len)) {
        printf("unable to load GPLX\n");
        return;
    }

    printf("GPLX #%d: length: %zu\n", id, len);

    gff_gpl_parse(gpl, len, NULL, 0);

    free(gpl);
}


static void print_gff_entry(gff_file_t *gff, gff_chunk_entry_t *entry) {
    uint32_t len;
    unsigned int *ids;
    gff_chunk_header_t chunk;
    void (*print_func)(gff_file_t *gff, unsigned int id);

    switch (entry->chunk_type) {
        case GFF_WIND: print_func = print_window; break;
        case GFF_BUTN: print_func = print_button; break;
        case GFF_GFFI: print_func = print_gffi; break;
        case GFF_TEXT: print_func = print_text; break;
        case GFF_ETME: print_func = print_etme; break;
        case GFF_SPIN: print_func = print_spin; break;
        case GFF_RDAT: print_func = print_rdat; break;
        case GFF_MONR: print_func = print_monr; break;
        case GFF_FONT: print_func = print_font; break;
        case GFF_MERR: print_func = print_merr; break;
        case GFF_CSEQ: print_func = print_cseq; break;
        case GFF_GSEQ: print_func = print_gseq; break;
        case GFF_PSEQ: print_func = print_pseq; break;
        case GFF_LSEQ: print_func = print_lseq; break;
        case GFF_BVOC: print_func = print_bvoc; break;
        case GFF_ACCL: print_func = print_accl; break;
        case GFF_EBOX: print_func = print_ebox; break;
        case GFF_APFM: print_func = print_frame; break;
        case GFF_ICON: print_func = print_icon; break;
        case GFF_BMP: print_func = print_bmp; break;
        case GFF_NAME: print_func = print_name; break;
        case GFF_IT1R: print_func = print_it1r; break;
        case GFF_PORT: print_func = print_port; break;
        case GFF_WALL: print_func = print_wall; break;
        case GFF_ETAB: print_func = print_etab; break;
        case GFF_GMAP: print_func = print_gmap; break;
        case GFF_RMAP: print_func = print_rmap; break;
        case GFF_TILE: print_func = print_tile; break;
        case GFF_RDFF: print_func = print_rdff; break;
        case GFF_SCMD: print_func = print_scmd; break;
        case GFF_OJFF: print_func = print_ojff; break;
        case GFF_MAS: print_func = print_mas; break;
        case GFF_GPL: print_func = print_gpl; break;
        case GFF_GPLX: print_func = print_gplx; break;
        case GFF_PSIN: print_func = print_psin; break;
        case GFF_PSST: print_func = print_psst; break;
        case GFF_SPST: print_func = print_spst; break;
        case GFF_CHAR: print_func = print_char; break;
        default:
            fprintf(stderr, "printer not written for '%c%c%c%c'\n",
                entry->chunk_type,
                entry->chunk_type >> 8,
                entry->chunk_type >> 16,
                entry->chunk_type >> 24
                );
            exit(1);
            break;
    }

    gff_load_id_list(gff, entry->chunk_type, &ids, &len);

    for (int i = 0; i < len; i++) {

        if (gff_find_chunk_header(gff, &chunk, entry->chunk_type, ids[i]) ) {
            fprintf(stderr, "Unable to read chunk %d: id = %d\n", i, ids[i]);
            exit(1);
        }

        print_func(gff, ids[i]);
    }

    free(ids);
}

extern void gffmod_print_entry(gff_file_t *gff, const char *name) {
    int32_t index = -1;

    // See if it is a number.
    if (name[0] == '0') {
        index = 0;
    } else {
        index = atoi(name);
        if (index == 0) { index = -1; }
    }

    // If not a number convert from type to index
    for (int i = 0; index == -1 && i < gff->num_types; i++) {
        gff_chunk_entry_t *entry = gff->chunks[i];
        if (!strncmp((char*)&entry->chunk_type, name, 4)) {
            index = i;
        }
    }

    if (index <= -1) {
        fprintf(stderr, "Unable to find entry.\n");
        exit(1);
    }

    if (index >= gff->num_types) {
        fprintf(stderr, "Entry '%d' out of range (0 - %d).\n", index, gff->num_types - 1);
        exit(1);
    }

    print_gff_entry(gff, gff->chunks[index]);
}

static int print_combat(ds1_combat_t *dc, const char *prefix) {
    //int16_t char_index; // 4, unconfirmed but looks right.
    //int16_t ready_item_index; // 8, to be cleared.
    //int16_t weapon_index; // 10, to be cleared
    //int16_t pack_index;   // 12, to be cleared
    //uint8_t data_block[8]; // just to shift down 8 bytes.
    //uint8_t allegiance;
    //uint8_t data;
    printf("%sName: %s\n", prefix, dc->name);
    printf("%sstats: {str: %d, dex: %d, con: %d, intel: %d, wis: %d, cha: %d}\n", prefix, dc->stats.str, dc->stats.dex, dc->stats.con, dc->stats.intel, dc->stats.wis, dc->stats.cha);
    printf("%scombat: {hp: %d, psp: %d, ac: %d, move: %d, status: %d, thac0: %d}\n", prefix, dc->hp, dc->psp, dc->ac, dc->move, dc->status, dc->thac0);
    printf("%sid: %d\n", prefix, dc->id);
    printf("%sspecial: {attack: %d, defense: %d}\n", prefix, dc->special_attack, dc->special_defense);
    printf("%sicon: %d\n", prefix, dc->icon);
    printf("%spriority: %d\n", prefix, dc->priority);
    printf("%sflags: %d\n", prefix, dc->flags);

    return EXIT_SUCCESS;
}

static int print_item(ds1_item_t *item, const char *prefix) {
    ds_item1r_t item1r;
    //int16_t  next;  // 4, for some internal book keeping.
    printf("%sid: %d\n", prefix, item->id); // points to OJFF
    printf("%squantity: %d\n", prefix, item->quantity);
    printf("%svalue: %d\n", prefix, item->value);
    printf("%spack_index: %d\n", prefix, item->pack_index);
    printf("%sitem_index: %d\n", prefix, item->item_index); // it1r
    if (!gff_manager_ds1_read_item1r(man, item->item_index, &item1r)) {
        print_item1r(&item1r, prefix);
    }
    printf("%sicon: %d\n", prefix, item->icon);
    printf("%scharges: %d\n", prefix, item->charges);
    printf("%sspecial: %d\n", prefix, item->special);
    printf("%sslot: %d\n", prefix, item->slot);
    printf("%sname: %d\n", prefix, item->name_idx);
    char name[64];
    if (!gff_manager_ds1_read_name(man, item->name_idx, name)) {
        printf("%s    %s\n", prefix, name);
    }
    printf("%sbonus: %d\n", prefix, item->bonus);
    printf("%spriority: %d\n", prefix, item->priority);
    return EXIT_SUCCESS;
}

extern int gffmod_print_rdff(gff_manager_t *man, gff_file_t *gff, const char *rdff_entry) {
    gff_rdff_t rdff;
    int32_t id;

    errno = 0;
    id = strtol(rdff_entry, NULL, 10);
    if (errno != 0) {
        printf("Unable to convert '%s' to a number.\n", rdff_entry);
        return EXIT_FAILURE;
    }

    if (gff_read_rdff(gff, id, &rdff)) {
        printf("Unable to load RDFF.\n");
        return EXIT_FAILURE;
    }

    printf("RDFF #%d:\n", id);
    printf("    header: {blocknum: %d, type: %d, index: %d, from: %d, len: %d}\n",
            rdff.header.blocknum,
            rdff.header.type,
            rdff.header.index,
            rdff.header.from,
            rdff.header.len);

    if (rdff.header.load_action == -1) {
        printf("RDFF load is -1!\n");
        return EXIT_FAILURE;
    }

    switch(rdff.header.load_action) {
        case RDFF_OBJECT:
            printf("    Type: Object/");
            switch(rdff.header.type) {
                case GFF_COMBAT_OBJECT:
                    printf("Combat\n");
                    print_combat((ds1_combat_t*)rdff.data, "        ");
                    break;
                case GFF_ITEM_OBJECT:
                    printf("ITEM\n");
                    print_item((ds1_item_t*)rdff.data, "        ");
                    break;
                case GFF_CHAR_OBJECT:
                    printf("CHAR\n");
                    break;
                case GFF_MINI_OBJECT:
                    printf("MINI\n");
                    break;
                default:
                    break;
            }
            break;
        case RDFF_CONTAINER:
            printf("CONTAINER\n");
            break;
        case RDFF_DATA:
            printf("DATA\n");
            break;
        case RDFF_NEXT:
            printf("NEXT\n");
            break;
        case RDFF_END:
            printf("END\n");
            break;
    }

    return EXIT_SUCCESS;
}
