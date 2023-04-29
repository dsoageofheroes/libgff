#include <stdlib.h>

#include "unity.h"
#include "gff/gff.h"
#include "gff/gfftypes.h"
#include "gff/manager.h"

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
    gff_manager_load_ds1(man, "ds1/");
    TS(gff_manager_free(man));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_smoke);
    RUN_TEST(test_load_ds1);
    return UNITY_END();
}
