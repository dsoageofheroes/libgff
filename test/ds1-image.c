#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/image.h"

#define TS(a) TEST_ASSERT(a == EXIT_SUCCESS)
#define BUF_SIZE (1<<12)

void setUp() {
}

void tearDown() {
}

void test_load(void) {
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RGN08.GFF"));
    TS(gff_palettes_read(f));
    TEST_ASSERT(0 == f->pals->len);
    //for (int i = 0; i < gff_get_number_of_types(f); i++) {
        //printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    //}
    //printf("--------\n");
    TS(gff_free(f));
}

void test_resource(void) {
    gff_frame_info_t info;
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));
    TS(gff_palettes_read(f));
    TEST_ASSERT(2 == f->pals->len);
    //for (int i = 0; i < gff_get_number_of_types(f); i++) {
        //printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    //}
    uint32_t len = 0;
    unsigned int* ids = gff_get_id_list(f, GFF_BMP, &len);
    TEST_ASSERT(len == 105);
    //printf("ids = %p\n", ids);
    //printf("ids = %d\n", ids[0]);
    TEST_ASSERT(1 == gff_get_frame_count(f, GFF_BMP, ids[1]));
    free(ids);

    ids = gff_get_id_list(f, GFF_ICON, &len);
    TEST_ASSERT(len == 292);
    //printf("ids = %d\n", ids[0]);
    TEST_ASSERT(2 == gff_get_frame_count(f, GFF_ICON, ids[0]));
    TEST_ASSERT(4 == gff_get_frame_count(f, GFF_ICON, ids[1]));
    TEST_ASSERT(3 == gff_get_frame_count(f, GFF_ICON, ids[2]));
    TS(gff_frame_info(f, GFF_ICON, ids[0], 0, &info));
    TEST_ASSERT_EQUAL(6, info.w);
    TEST_ASSERT_EQUAL(8, info.h);
    unsigned char *data = gff_get_frame_rgba_palette(f, GFF_ICON, ids[0], 0, f->pals->palettes + 0);
    printf("ID: %d, data: %p\n", ids[0], data);
    TEST_ASSERT_NOT_NULL(data);
    free(data);
    data = gff_get_frame_rgba_palette(f, GFF_ICON, ids[0], 5, f->pals->palettes + 0);
    TEST_ASSERT_NULL(data);
    data = gff_get_frame_rgba_palette(f, GFF_ICON, 99999, 0, f->pals->palettes + 0);
    TEST_ASSERT_NULL(data);
    free(ids);

    data = gff_create_font_rgba(f, 'A', 0x00FF00, 0x000000);
    TEST_ASSERT_NOT_NULL(data);
    free(data);

    TS(gff_free(f));
}

void test_gpldata(void) {
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/GPLDATA.GFF"));
    TS(gff_palettes_read(f));
    TEST_ASSERT(45 == f->pals->len);
    //for (int i = 0; i < gff_get_number_of_types(f); i++) {
        //printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    //}
    TS(gff_free(f));
}

void test_palette(void) {
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));
    TEST_ASSERT(2 == gff_get_palette_len(f));
    for (ssize_t i = 0; i < gff_get_palette_len(f); i++) {
        TEST_ASSERT(i == gff_get_palette_id(f, i));
    }
    TEST_ASSERT(-1 == gff_get_palette_id(f, gff_get_palette_len(f)));

    TS(gff_free(f));
    f = NULL;

    // Now negative
    TEST_ASSERT(-1 == gff_get_palette_len(f));
    TEST_ASSERT(-1 == gff_get_palette_id(f, 0));

    f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RGN08.GFF"));
    TEST_ASSERT(-1 == gff_get_palette_id(f, 0));
    TS(gff_free(f));
}

void test_neg(void) {
    gff_chunk_header_t chunk;
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));
    
    gff_find_chunk_header(f, &chunk, GFF_ICON, 0);

    TS(gff_free(f));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_load);
    RUN_TEST(test_resource);
    RUN_TEST(test_gpldata);
    RUN_TEST(test_neg);
    return UNITY_END();
}
