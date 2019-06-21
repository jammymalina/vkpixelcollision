#include "unity.h"

#include <stdio.h>

#include "../src/collections/vector.h"

typedef struct int_vector VECTOR(int) int_vector;

void test_vector_init() {
    int_vector v;
    vector_init(&v);
    bool status = vector_reserve(&v, 100);
    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for vector");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(100, v.cap, "Capacity of vector"
        " is insufficient");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, v.size, "Size of an empty vector should"
        " be zero");
    vector_destroy(&v);
}

void test_vector_insert() {
    int_vector v1, v2;
    vector_init(&v1);
    vector_init(&v2);

    bool status = vector_reserve(&v1, 100) && vector_reserve(&v2, 100);
    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for vector");

    bool push_status = true;
    push_status = vector_push(&v1, 10) && vector_push(&v1, 20) &&
        vector_push(&v1, 30) && vector_push(&v1, 40) && vector_push(&v1, 50) &&
        vector_push(&v1, 60);

    TEST_ASSERT_MESSAGE(push_status, "Unable to add all the data to vector");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, v1.size, "Size of a vector should be 6 "
        " after pushing 6 elements");
    int expected[] = { 10, 20, 30, 40, 50, 60 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, v1.data, 6, "The array does"
        " not equal vector data");

    vector_destroy(&v1);
    vector_destroy(&v2);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_vector_init);
    RUN_TEST(test_vector_insert);

    return UNITY_END();
}
