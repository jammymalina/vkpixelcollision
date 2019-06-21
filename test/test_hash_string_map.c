#include "unity.h"

#include "../src/collections/hash_string_map.h"

void test_init_hash_string_map() {
    int a = 1;
    TEST_ASSERT( a == 1 );
}


int main() {
    UNITY_BEGIN();

    RUN_TEST(test_init_hash_string_map);

    return UNITY_END();
}

