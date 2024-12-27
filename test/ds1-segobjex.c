#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/rdff.h"
#include "gff/scmd.h"

#define TS(a) TEST_ASSERT(a == EXIT_SUCCESS)
#define TF(a) TEST_ASSERT(a == EXIT_FAILURE)
#define BUF_SIZE (1<<12)

void setUp() {
    //gff_load_directory("/home/pwest/dosbox/DARKSUN");
}

void tearDown() {
    gff_cleanup();
}

void test_rdff(void) {
    gff_file_t *f = gff_allocate();
    //gff_chunk_header_t chunk;
    gff_rdff_t rdff;
    uint32_t len;
    //char buf[BUF_SIZE];
    //char buf2[BUF_SIZE];
    gff_object_t so;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/SEGOBJEX.GFF"));
    //for (int i = 0; i < gff_get_number_of_types(f); i++) {
        //printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    //}

    TF(gff_read_rdff(f, 0, &rdff));

    unsigned int* ids;
    gff_load_id_list(f, GFF_RDFF, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        TS(gff_read_rdff(f, ids[i], &rdff));
        TS(gff_rdff_to_object(&rdff, &so));
        //printf("type: %d, id: %d\n", so.type, ids[i]);
    }
    free(ids);

    TS(gff_read_rdff(f, 32003, &rdff));
    TS(gff_rdff_to_object(&rdff, &so));
    TEST_ASSERT(so.type == 1);

    TS(gff_free(f));
}

void test_scmd(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len, slen;
    gff_scmd_t *scmd;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/SEGOBJEX.GFF"));
    for (int i = 0; i < gff_get_number_of_types(f); i++) {
        //printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    }

    unsigned int* ids;
    gff_load_id_list(f, GFF_SCMD, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        TS(gff_scmd_read(f, ids[i], &scmd, &slen));
        free(scmd);
        //TS(gff_read_rdff(f, ids[i], &rdff));
        //TS(gff_rdff_to_object(&rdff, &so));
        //printf("type: %d, id: %d\n", so.type, ids[i]);
    }
    free(ids);

    TS(gff_scmd_read(f, 256, &scmd, &len));
    free(scmd);

    TS(gff_free(f));
}

void test_ojff(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    gff_ojff_t ojff;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/SEGOBJEX.GFF"));
    for (int i = 0; i < gff_get_number_of_types(f); i++) {
        printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    }

    unsigned int* ids;
    gff_load_id_list(f, GFF_OJFF, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        TS(gff_ojff_read(f, ids[i], &ojff));
    }
    free(ids);

    TS(gff_ojff_read(f, 32003, &ojff));
    TEST_ASSERT(ojff.xpos == 162);

    TS(gff_free(f));
}

void test_item(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    ds1_item_t item;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/SEGOBJEX.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_RDFF, &ids, &len);
    int item_count = 0;
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        //TS(gff_ojff_read(f, ids[i], &ojff));
        if (!gff_item_read(f, ids[i], &item)) {
            item_count++;
            TEST_ASSERT(item.id < 0);
        }
    }
    free(ids);

    TEST_ASSERT(item_count == 755);

    TS(gff_free(f));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_rdff);
    RUN_TEST(test_scmd);
    RUN_TEST(test_ojff);
    RUN_TEST(test_item);
    return UNITY_END();
}
