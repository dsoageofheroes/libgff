#include <stdlib.h>

#include "unity.h"
#include "gff.h"
#include "gfftypes.h"
#include "gui.h"

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

    unsigned int* ids = gff_get_id_list(f, GFF_TEXT, &len);
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

    unsigned int* ids = gff_get_id_list(f, GFF_RDAT, &len);
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
    ds_font_t *font;
    ds_char_t *ds_char = NULL;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    TS(gff_read_font(f, 100, &font));
    int count = 0;
    for (int c = 0; c < MAX_CHARS; c++ ) {
        ds_char = (ds_char_t*)(((uint8_t*)font) + font->char_offset[c]);
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

    unsigned int* ids = gff_get_id_list(f, GFF_SPIN, &len);
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

    unsigned int* ids = gff_get_id_list(f, GFF_EBOX, &len);
    for (int i = 0; i < len; i++) {
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
        //TS(gff_read_button(f, ids[i], &button));
        TS(gff_read_ebox(f, ids[i], &ebox));
        //printf("buf = '%s'\n", buf);
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

    unsigned int* ids = gff_get_id_list(f, GFF_WIND, &len);
    for (int i = 0; i < len; i++) {
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

    unsigned int* ids = gff_get_id_list(f, GFF_BUTN, &len);
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
    gff_frame_t frame;

    TEST_ASSERT_NOT_NULL(f);
    TS(gff_open(f, "ds1/RESOURCE.GFF"));

    unsigned int* ids = gff_get_id_list(f, GFF_APFM, &len);
    for (int i = 0; i < len; i++) {
        //printf("->%d\n", ids[i]);
        //TS(gff_read_text(f, ids[i], buf, BUF_SIZE));
        //(gff_read_ebox(f, ids[i], buf, BUF_SIZE));
        TS(gff_read_frame(f, ids[i], &frame));
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
    return UNITY_END();
}
