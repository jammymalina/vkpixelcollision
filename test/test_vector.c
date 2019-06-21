#include "unity.h"

#include "../src/collections/vector.h"

void test_vector_init() {
    int a = 1;
    TEST_ASSERT( a == 1 );
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_vector_init);

    return UNITY_END();
}
