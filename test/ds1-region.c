#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/region.h"

#define TS(a) TEST_ASSERT(a == EXIT_SUCCESS)
#define BUF_SIZE (1<<12)

void setUp() {
}

void tearDown() {
}

void test_rgn08(void) {
    uint32_t amt;
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT(EXIT_FAILURE == gff_region_get_num_objects(f, &amt));

    TS(gff_open(f, "ds1/RGN08.GFF"));
    //for (int i = 0; i < gff_get_number_of_types(f); i++) {
        //printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    //}

    //for (int i = 0; i < 526; i++) {
        //so_object_t *so = gff_object_inspect(f, i);
    //}
//extern so_object_t* gff_object_inspect(gff_file_t *f, int res_id) {
    //printf("->%d\n", gff_region_get_object_frame_count(f, 1));
//extern int gff_region_get_object_frame_count(gff_file_t *f, int res_id, int obj_id) {
    gff_region_t* region = gff_region_load(f);
    TEST_ASSERT_NOT_NULL(region);

    int flags_found = 0x0;
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLUMNS; j++) {
            TEST_ASSERT(region->tiles[i][j] > 0);
            TEST_ASSERT(region->tile_ids[region->tiles[i][j] - 1] > 0);
            flags_found |= region->flags[i][j];
        }
    }

    TEST_ASSERT(flags_found != 0x0);

    TEST_ASSERT(region->num_objects == 526);
    TEST_ASSERT(region->etab[1].xpos != 0);
    TEST_ASSERT(region->etab[1].ypos != 0);

    TS(gff_region_free(region));

    TS(gff_free(f));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_rgn08);
    return UNITY_END();
}
