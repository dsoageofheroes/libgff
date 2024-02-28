#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"

#define TS(a) TEST_ASSERT(a == EXIT_SUCCESS)
#define BUF_SIZE (1<<12)

void setUp() {
    //gff_load_directory("/home/pwest/dosbox/DARKSUN");
}

void tearDown() {
    gff_cleanup();
}

void test_smoke(void) {
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_free(f));
}

void test_load(void) {
    gff_file_t *f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RGN08.GFF"));
    TS(gff_free(f));
}

void test_resource_read(void) {
    gff_file_t *f = gff_allocate();
    gff_chunk_header_t chunk;
    uint32_t len;
    char buf[BUF_SIZE];
    char buf2[BUF_SIZE];

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RGN08.GFF"));
    TEST_ASSERT(5 == gff_get_number_of_types(f));
    TEST_ASSERT(GFF_GFFI == gff_get_type_id(f, 0));
    TEST_ASSERT(GFF_ETAB == gff_get_type_id(f, 1));
    TEST_ASSERT(GFF_GMAP == gff_get_type_id(f, 2));
    TEST_ASSERT(GFF_RMAP == gff_get_type_id(f, 3));
    TEST_ASSERT(GFF_TILE == gff_get_type_id(f, 4));

    unsigned int* list;
    gff_load_id_list(f, GFF_GFFI, &list, &len);
    TEST_ASSERT(len == 1);
    for (uint32_t i = 0; i < len; i++) {
        TEST_ASSERT(list[i] == i);
    }
    free(list);

    gff_load_id_list(f, GFF_ETAB, &list, &len);
    TEST_ASSERT(len == 1);
    for (uint32_t i = 0; i < len; i++) {
        TEST_ASSERT(list[i] == i + 8);
    }
    free(list);

    gff_load_id_list(f, GFF_GMAP, &list, &len);
    TEST_ASSERT(len == 1);
    for (uint32_t i = 0; i < len; i++) {
        TEST_ASSERT(list[i] == i + 8);
    }
    free(list);

    gff_load_id_list(f, GFF_RMAP, &list, &len);
    TEST_ASSERT(len == 1);
    for (uint32_t i = 0; i < len; i++) {
        TEST_ASSERT(list[i] == i + 8);
    }
    free(list);

    gff_load_id_list(f, GFF_TILE, &list, &len);
    TEST_ASSERT(len == 168);
    for (uint32_t i = 0; i < len; i++) {
        TEST_ASSERT(list[i] == i + 1);
        TEST_ASSERT(gff_find_chunk_header(f, &chunk, GFF_TILE, list[i]) == EXIT_SUCCESS);
        TEST_ASSERT(chunk.length > 0);
        buf[0] = 0;
        TEST_ASSERT(gff_read_chunk(f, &chunk, buf, chunk.length) == chunk.length);
        TEST_ASSERT(gff_read_raw_bytes(f, GFF_TILE, list[i], buf2, BUF_SIZE));
        TEST_ASSERT_EQUAL_HEX8_ARRAY(buf, buf2, chunk.length);
        TEST_ASSERT(buf[0] != 0);
    }
    free(list);

    TEST_ASSERT(5 == gff_get_number_of_types(f));

    TEST_ASSERT(0 == gff_get_palette_len(f));

    TS(gff_free(f));
}

void test_resource_read_negative(void) {
    gff_file_t *f = NULL;
    gff_chunk_header_t chunk;
    uint32_t len = 0;
    char buf[BUF_SIZE];
    char buf2[BUF_SIZE];

    TEST_ASSERT_NULL(f);
    TS(!gff_open(f, "ds1/RGN08.GFF"));
    TEST_ASSERT(-1 == gff_get_number_of_types(f));
    TEST_ASSERT(-1 == gff_get_type_id(f, 0));
    TEST_ASSERT(-1 == gff_get_type_id(f, 1));
    TEST_ASSERT(-1 == gff_get_type_id(f, 2));
    TEST_ASSERT(-1 == gff_get_type_id(f, 3));
    TEST_ASSERT(-1 == gff_get_type_id(f, 4));

    uint32_t *list;
    gff_load_id_list(f, GFF_GFFI, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_ETAB, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_GMAP, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_RMAP, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_TILE, &list, &len);
    TEST_ASSERT_NULL(list);
    TEST_ASSERT(gff_find_chunk_header(f, &chunk, GFF_TILE, 0) == EXIT_FAILURE);
    TEST_ASSERT(gff_read_chunk(f, &chunk, buf, chunk.length) == chunk.length);
    TEST_ASSERT(gff_read_raw_bytes(f, GFF_TILE, 0, buf2, BUF_SIZE) == 0);

    TEST_ASSERT(-1 == gff_get_number_of_types(f));

    TEST_ASSERT(-1 == gff_get_palette_len(f));

    TEST_ASSERT(gff_free(f) == EXIT_FAILURE);

    f = gff_allocate();
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT(-1 == gff_get_number_of_types(f));
    TEST_ASSERT(-1 == gff_get_type_id(f, 0));
    TEST_ASSERT(-1 == gff_get_type_id(f, 1));
    TEST_ASSERT(-1 == gff_get_type_id(f, 2));
    TEST_ASSERT(-1 == gff_get_type_id(f, 3));
    TEST_ASSERT(-1 == gff_get_type_id(f, 4));

    gff_load_id_list(f, GFF_GFFI, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_ETAB, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_GMAP, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_RMAP, &list, &len);
    TEST_ASSERT_NULL(list);

    gff_load_id_list(f, GFF_TILE, &list, &len);
    TEST_ASSERT_NULL(list);
    TEST_ASSERT(gff_find_chunk_header(f, &chunk, GFF_TILE, 0) == EXIT_FAILURE);
    TEST_ASSERT(gff_read_chunk(f, &chunk, buf, chunk.length) == chunk.length);
    TEST_ASSERT(gff_read_raw_bytes(f, GFF_TILE, 0, buf2, BUF_SIZE) == 0);

    TEST_ASSERT(-1 == gff_get_number_of_types(f));

    TEST_ASSERT(-1 == gff_get_palette_len(f));

    TEST_ASSERT(gff_free(f) == EXIT_SUCCESS);
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_smoke);
    RUN_TEST(test_load);
    RUN_TEST(test_resource_read);
    RUN_TEST(test_resource_read_negative);
    RUN_TEST(test_palette);
    return UNITY_END();
}
