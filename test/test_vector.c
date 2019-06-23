#include "unity.h"

#include <stdint.h>
#include <stdio.h>

#include "../src/collections/vector.h"

typedef struct int_vector VECTOR(int32_t) int_vector;

void test_vector_init() {
    int_vector v;
    vector_init(&v);

    bool status = vector_reserve(&v, 100);
    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for vector");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(100, v.cap, "Capacity of vector"
        " is insufficient");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, v.size, "Size of an empty vector should"
        " be zero");

    vector_clear(&v);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, v.size, "Size of an empty vector should"
        " be zero after destroying");
}

void test_vector_insert() {
    int_vector v1, v2;
    vector_init(&v1);
    vector_init(&v2);

    bool status = vector_reserve(&v1, 100) && vector_reserve(&v2, 10);
    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for vector");

    // Push element one by one
    bool push_status = vector_push(&v1, 10) && vector_push(&v1, 20) &&
        vector_push(&v1, 30) && vector_push(&v1, 40) && vector_push(&v1, 50) &&
        vector_push(&v1, 60);

    TEST_ASSERT_MESSAGE(push_status, "Unable to add all the data to vector");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, v1.size, "Size of a vector should be 6"
        " after pushing 6 elements");
    int32_t expected[] = { 10, 20, 30, 40, 50, 60 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(expected, v1.data, 6, "The array does"
        " not equal vector data");

    // Insert all
    const int32_t items[] = { 23, 32, 30, 41, 78, 90, 200, 210, 220, 248, 44,
        44, 82 };
    push_status = vector_push_all(&v2, items, 13);
    TEST_ASSERT_MESSAGE(push_status, "Unable to add all the data to vector");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(13, v2.size, "Size of a vector should be"
        " 13 after pushing 13 elements");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(13, v2.cap, "Capacity of vector"
        " should have increased after data insertion");
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items, v2.data, 13, "The array does"
        " not equal vector data");

    // Insert element index 0
    push_status = vector_insert(&v2, 0, 42);
    TEST_ASSERT_MESSAGE(push_status, "Unable to insert data to index 0");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(14, v2.size, "Size of a vector should be"
        " 14 after insertion of 1 element");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(14, v2.cap, "Capacity of vector"
        " should have increased after element insertion");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(42, v2.data[0], "Inserting to index 0"
        " failed");
    const int32_t items_insert_0[] = { 42, 23, 32, 30, 41, 78, 90, 200, 210,
        220, 248, 44, 44, 82 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_insert_0, v2.data, 14, "The"
        " array does not equal vector data");

    // Insert element at index within vector
    push_status = vector_insert(&v2, 5, 63);
    TEST_ASSERT_MESSAGE(push_status, "Unable to insert data to index within"
        " vector");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(15, v2.size, "Size of a vector should be"
        " 15 after insertion of 1 element");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(15, v2.cap, "Capacity of vector"
        " should have increased after element insertion");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(63, v2.data[5], "Inserting to index within"
        " vector failed");
    const int32_t items_insert_with_vec[] = { 42, 23, 32, 30, 41, 63, 78, 90,
        200, 210, 220, 248, 44, 44, 82 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_insert_with_vec, v2.data, 15,
        "The array does not equal vector data");

    // push after all insert operations
    push_status = vector_push(&v2, 99);
    TEST_ASSERT_MESSAGE(push_status, "Unable to push to vector after insert"
        " operations");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(16, v2.size, "Size of a vector should be"
        " 16 after push");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(16, v2.cap, "Capacity of vector"
        " should have increased after element insertion");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(99, v2.data[15], "Push after insertion"
        " failed");
    const int32_t items_push_after_insert[] = { 42, 23, 32, 30, 41, 63, 78, 90,
        200, 210, 220, 248, 44, 44, 82, 99 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_push_after_insert, v2.data, 16,
        "The array does not equal vector data");

    vector_clear(&v1);
    vector_clear(&v2);

    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, v1.size, "Size of an empty vector should"
        " be zero after destroying");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, v2.size, "Size of an empty vector should"
        " be zero after destroying");
}

void test_vector_search() {
    int_vector v1;
    vector_init(&v1);

    const int32_t items[] = { 23, 32, 30, 41, 78, 90, 200, 210, 220, 248, 44,
        44, 82 };
    bool status = vector_reserve(&v1, 100) && vector_push_all(&v1, items, 13);
    TEST_ASSERT_MESSAGE(status, "Unable to init vector with data");

    // Vector contains these elements
    ssize_t idx = -1;
    vector_index_of(&v1, 23, &idx);
    TEST_ASSERT_EQUAL_INT64_MESSAGE(0, idx, "Vector contains element");
    vector_index_of(&v1, 44, &idx);
    TEST_ASSERT_EQUAL_INT64_MESSAGE(10, idx, "Vector contains element");
    vector_index_of(&v1, 82, &idx);
    TEST_ASSERT_EQUAL_INT64_MESSAGE(12, idx, "Vector contains element");

    // Vector does not contain these elements
    vector_index_of(&v1, 528, &idx);
    TEST_ASSERT_EQUAL_INT64_MESSAGE(-1, idx, "Vector doesn't contain element");
    vector_index_of(&v1, 99, &idx);
    TEST_ASSERT_EQUAL_INT64_MESSAGE(-1, idx, "Vector doesn't contain element");
    vector_index_of(&v1, -1, &idx);
    TEST_ASSERT_EQUAL_INT64_MESSAGE(-1, idx, "Vector doesn't contain element");

    vector_clear(&v1);
}

void test_vector_delete() {
    int_vector v1;
    vector_init(&v1);

    const int32_t items[] = { 23, 32, 30, 41, 78, 90, 200, 210, 220, 248, 44,
        44, 82 };
    bool status = vector_reserve(&v1, 100) && vector_push_all(&v1, items, 13);
    TEST_ASSERT_MESSAGE(status, "Unable to init vector with data");


    // Removal of first element
    vector_remove(&v1, 0);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(12, v1.size, "Size of a vector should be"
        " 12 after removal of element");
    const int32_t items_rm_0[] = { 32, 30, 41, 78, 90, 200, 210, 220, 248, 44,
        44, 82 };
    TEST_ASSERT_EQUAL_INT32_MESSAGE(32, v1.data[0], "First element should be 32"
        " after removal");
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_rm_0, v1.data, 12, " The array"
        " does not equal vector data after removal");

    // Removal of element
    vector_remove(&v1, 5);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(11, v1.size, "Size of a vector should be"
        " 11 after removal of element");
    const int32_t items_rm_5[] = { 32, 30, 41, 78, 90, 210, 220, 248, 44, 44,
        82 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_rm_5, v1.data, 11, " The array"
        " does not equal vector data after removal");

    // Removal of last element
    vector_remove(&v1, 10);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(10, v1.size, "Size of a vector should be"
        " 10 after removal of element");
    const int32_t items_rm_10[] = { 32, 30, 41, 78, 90, 210, 220, 248, 44, 44 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_rm_10, v1.data, 10, " The array"
        " does not equal vector data after removal");

    // Removal noshrink
    vector_remove_noshrink(&v1, 2);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(9, v1.size, "Size of a vector should be"
        " 9 after removal of element");
    const int32_t items_rm_2[] = { 32, 30, 78, 90, 210, 220, 248, 44, 44 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_rm_2, v1.data, 9, " The array"
        " does not equal vector data after removal");

    // Removal of slice
    vector_remove_slice(&v1, 2, 5);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(4, v1.size, "Size of a vector should be"
        " 9 after slice removal");
    const int32_t items_rm_slice[] = { 32, 30, 44, 44 };
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(items_rm_slice, v1.data, 4, "The"
        " array does not equal vector data after removal");

    // Removal swap
    vector_swap_remove(&v1, 1);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(3, v1.size, "Size of a vector should be"
        " 8 after removal of element");
    ssize_t idx = -1;
    vector_index_of(&v1, 30, &idx);
    TEST_ASSERT_MESSAGE(idx == -1, "Vector still contains removed element");

    vector_clear(&v1);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_vector_init);
    RUN_TEST(test_vector_insert);
    RUN_TEST(test_vector_search);
    RUN_TEST(test_vector_delete);

    return UNITY_END();
}
