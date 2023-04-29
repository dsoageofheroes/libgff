#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/manager.h"
#include "gff/map.h"

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

void test_load_ds1_map_objects(void) {
    uint32_t num_objects;
    gff_region_object_t obj;
    gff_manager_t *man = gff_manager_create();

    TEST_ASSERT_NOT_NULL(man);

    TS(gff_manager_load_ds1(man, "ds1/"));

    TEST_ASSERT(gff_map_get_num_objects(man->ds1.regions[42], &num_objects) == EXIT_SUCCESS);
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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_smoke);
    RUN_TEST(test_load_ds1);
    RUN_TEST(test_load_ds1_map_objects);
    return UNITY_END();
}
