#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/gui.h"
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
    //gff_rdff_t rdff;
    //uint32_t len;
    //gff_object_t so;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/GPLDATA.GFF"));
    for (int i = 0; i < gff_get_number_of_types(f); i++) {
        printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    }

    //unsigned int* ids = gff_get_id_list(f, GFF_RDFF, &len);
    //for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        //TS(gff_rdff_load(f, ids[i], &rdff));
        //TS(gff_rdff_to_object(&rdff, &so));
        //printf("type: %d, id: %d\n", so.type, ids[i]);
    //}
    //free(ids);

    TS(gff_free(f));
}

void test_pal(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    gff_raw_palette_t pal;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/GPLDATA.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_PAL, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("PAL: %d\n", ids[i]);
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
        TS(gff_read_raw_pal(f, ids[i], &pal));
        //int i = 240;
        //printf("%d: 0x%02x%02x%02x\n", i, pal.color[i].r, pal.color[i].g, pal.color[i].b);
        //for (int i = 0; i < 256; i++) {
            //printf("%d: 0x%02x%02x%02x\n", i, pal.color[i].r, pal.color[i].g, pal.color[i].b);
        //}
        //printf("len = %d\n", button.textlen);
        //printf("buf = '%s'\n", buf);
    }
    free(ids);

    TS(gff_free(f));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_rdff);
    RUN_TEST(test_pal);
    return UNITY_END();
}
