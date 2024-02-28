#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/gui.h"

#define TS(a) TEST_ASSERT(a == EXIT_SUCCESS)
#define BUF_SIZE (1<<12)

void setUp() {
    //gff_load_directory("/home/pwest/dosbox/DARKSUN");
}

void tearDown() {
    gff_cleanup();
}

void test_resource(void) {
    gff_file_t *f = gff_allocate();

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));
    for (int i = 0; i < gff_get_number_of_types(f); i++) {
        printf("'%s'\n", gff_type_to_str(gff_get_type_id(f, i)));
    }

    TS(gff_free(f));
}

void test_text(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    char buf[BUF_SIZE];

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_TEXT, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        buf[0] = '\0';
        TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //printf("buf = '%s'\n", buf);
        TEST_ASSERT(buf[0] != '\0');
    }
    free(ids);

    TS(gff_free(f));
}

void test_rdat(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    char buf[BUF_SIZE];

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_RDAT, &ids, &len);
    for (int i = 0; i < 33; i++) {
        //printf("->%d\n", ids[i]);
        buf[0] = '\0';
        TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        (gff_read_text(f, ids[i], buf, BUF_SIZE));
        //printf("buf = '%s'\n", buf);
        TEST_ASSERT(buf[0] != '\0');
    }
    free(ids);

    TS(gff_free(f));
}

void test_font(void) {
    gff_file_t *f = gff_allocate();
    gff_font_t *font;
    gff_char_t *ds_char = NULL;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    TS(gff_read_font(f, 100, &font));
    int count = 0;
    for (int c = 0; c < MAX_CHARS; c++ ) {
        ds_char = (gff_char_t*)(((uint8_t*)font) + font->char_offset[c]);
        if (ds_char->width > 0) {
            count++;
        }
    }
    free(font);

    TEST_ASSERT(count > 80);

    TS(gff_free(f));
}

void test_spin(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    char buf[BUF_SIZE];

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_SPIN, &ids, &len);
    for (int i = 0; i < len; i++) {
        buf[0] = '\0';
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        TS(gff_read_spin(f, ids[i], buf, BUF_SIZE));
        //printf("buf = '%s'\n", buf);
        TEST_ASSERT(buf[0] != '\0');
    }
    free(ids);

    TS(gff_free(f));
}

void test_ebox(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    gff_ebox_t ebox;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_EBOX, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("EBOX: %d, %d, %lu\n", ids[i], gff_read_ebox(f, ids[i], &ebox), sizeof(gff_ebox_t));
        TS(gff_read_ebox(f, ids[i], &ebox));
    }
    free(ids);

    TS(gff_free(f));
}

void test_window(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    gff_window_t *win;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_WIND, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("ids[%d] = %d\n", i, ids[i]);
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
        TS(gff_read_window(f, ids[i], &win));
        //printf("buf = '%s'\n", buf);
        free(win);
    }
    free(ids);

    TS(gff_free(f));
}

void test_button(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    gff_button_t button;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_BUTN, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
        TS(gff_read_button(f, ids[i], &button));
        //printf("len = %d\n", button.textlen);
        //printf("buf = '%s'\n", buf);
    }
    free(ids);

    TS(gff_free(f));
}

void test_frame(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    //gff_frame_t frame;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_APFM, &ids, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
        //TS(gff_read_frame(f, ids[i], &frame));
        //printf("len = %d\n", button.textlen);
        //printf("buf = '%s'\n", buf);
    }
    free(ids);

    TS(gff_free(f));
}

void test_pal(void) {
    gff_file_t *f = gff_allocate();
    uint32_t len;
    gff_raw_palette_t pal;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids;
    gff_load_id_list(f, GFF_PAL, &ids, &len);
    for (int i = 0; i < len; i++) {
        printf("PAL: %d\n", ids[i]);
        TS(gff_read_raw_pal(f, ids[i], &pal));

        //int i = 239;
        //printf("%d: 0x%02x%02x%02x\n", i, pal.color[i].r, pal.color[i].g, pal.color[i].b);
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
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
    RUN_TEST(test_resource);
    RUN_TEST(test_text);
    RUN_TEST(test_rdat);
    RUN_TEST(test_font);
    RUN_TEST(test_spin);
    RUN_TEST(test_ebox);
    RUN_TEST(test_window);
    RUN_TEST(test_button);
    RUN_TEST(test_frame);
    RUN_TEST(test_pal);
    return UNITY_END();
}
