#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/image.h"
#include "gff/manager.h"
#include "gff/region.h"

#define TS(a) TEST_ASSERT(a == EXIT_SUCCESS)
#define BUF_SIZE (1<<12)

void setUp() {
}

void tearDown() {
}

void test_smoke(void) {
    gff_manager_t *man = gff_manager_create();
    TEST_ASSERT_NOT_NULL(man);
    TS(gff_manager_free(man));
}

void test_load_ds1(void) {
    gff_manager_t *man = gff_manager_create();
    TEST_ASSERT_NOT_NULL(man);
    TS(gff_manager_load_ds1(man, "ds1/"));
    TS(gff_manager_free(man));
}

void test_load_ds1_region_objects(void) {
    uint32_t num_objects;
    gff_region_object_t obj;
    gff_manager_t *man = gff_manager_create();

    TEST_ASSERT_NOT_NULL(man);

    TS(gff_manager_load_ds1(man, "ds1/"));

    TEST_ASSERT(gff_region_get_num_objects(man->ds1.regions[42], &num_objects) == EXIT_SUCCESS);
    TEST_ASSERT(num_objects > 0);

    for (int i = 0; i < gff_get_number_of_types(man->ds1.regions[42]); i++) {
        printf("'%s'\n", gff_type_to_str(gff_get_type_id(man->ds1.regions[42], i)));
    }

    for (int i = 0; i < num_objects; i++) {
        TS(gff_manager_create_ds1_region_object(man, 42, i, &obj));
        TS(gff_region_object_free(&obj));
    }

    TS(gff_manager_free(man));
}

void test_load_ds1_region(void) {
    gff_manager_t *man = gff_manager_create();
    gff_region_t *reg = NULL;

    TEST_ASSERT_NOT_NULL(man);

    TS(gff_manager_load_ds1(man, "ds1/"));
    reg = gff_region_load(man->ds1.regions[42]);
    TS(gff_manager_load_region_objects(man, reg));

    gff_region_free(reg);
    TS(gff_manager_free(man));
}

void test_images(void) {
    gff_manager_t *man = gff_manager_create();
    //gff_region_t *reg = NULL;

    TEST_ASSERT_NOT_NULL(man);

    TS(gff_manager_load_ds1(man, "ds1/"));

    gff_frame_info_t info;
    TS(gff_frame_info(man->ds1.resource, GFF_ICON, 100, 0, &info));
    printf("%d x %d\n", info.w, info.h);
    unsigned char *data = gff_get_frame_rgba_palette(man->ds1.resource, GFF_ICON, 100, 0, man->ds1.resource->pals->palettes + 0);
    printf("data = %p\n", data);
    free(data);
    TS(gff_manager_free(man));
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_smoke);
    RUN_TEST(test_load_ds1);
    RUN_TEST(test_load_ds1_region_objects);
    RUN_TEST(test_load_ds1_region);
    RUN_TEST(test_images);
    return UNITY_END();
}
