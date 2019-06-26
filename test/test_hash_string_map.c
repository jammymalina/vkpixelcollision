#include "unity.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "./uuids.h"

#include "../src/collections/hash_string_map.h"

typedef struct hash_int_map HASH_STRING_MAP(int32_t) hash_int_map;

// DEBUG
// for (size_t i = 0; i < m.nodes.cap; ++i)
//     printf("%ld: %s -> %d\n", i, m.nodes.data[i].key, m.nodes.data[i].value);

void test_init_hash_string_map() {
    hash_int_map m;
    hash_string_map_init(&m);

    bool status = hash_string_map_reserve(&m, 100);

    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for map");
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(100,
        hash_string_map_get_capacity(&m), "Capacity of map is insufficient");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(0, hash_string_map_get_size(&m),
        "Size of an empty map should be zero");

    hash_string_map_destroy(&m);
}

void test_hash_string_map_insert() {
    hash_int_map m;
    hash_string_map_init(&m);

    bool status = hash_string_map_reserve(&m, 20);

    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for map");

    bool add_status = hash_string_map_add(&m, "spring", 10) &&
        hash_string_map_add(&m, "autumn", 20) &&
        hash_string_map_add(&m, "wheel", 30) &&
        hash_string_map_add(&m, "umbrella", 40) &&
        hash_string_map_add(&m, "roomba", 50) &&
        hash_string_map_add(&m, "salsa", 100);

    TEST_ASSERT_MESSAGE(add_status, "Unable to add data to map");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, hash_string_map_get_size(&m), "Size of"
        " a map should be 6 after adding 6 elements");

    // we should be able to get search the map
    bool is_in_map = hash_string_map_has(&m, "roomba");
    TEST_ASSERT_MESSAGE(is_in_map, "Element should be in the map");
    is_in_map = hash_string_map_has(&m, "wheel");
    TEST_ASSERT_MESSAGE(is_in_map, "Element should be in the map");
    is_in_map = hash_string_map_has(&m, "salsa");
    TEST_ASSERT_MESSAGE(is_in_map, "Element should be in the map");
    is_in_map = hash_string_map_has(&m, "spring");
    TEST_ASSERT_MESSAGE(is_in_map, "Element should be in the map");
    is_in_map = hash_string_map_has(&m, "autumn");
    TEST_ASSERT_MESSAGE(is_in_map, "Element should be in the map");
    is_in_map = hash_string_map_has(&m, "umbrella");
    TEST_ASSERT_MESSAGE(is_in_map, "Element should be in the map");

    // we should not get false positives
    is_in_map = hash_string_map_has(&m, "key1");
    TEST_ASSERT_FALSE_MESSAGE(is_in_map, "Element shouldn't be in the map");
    is_in_map = hash_string_map_has(&m, "romba");
    TEST_ASSERT_FALSE_MESSAGE(is_in_map, "Element shouldn't be in the map");
    is_in_map = hash_string_map_has(&m, "key10");
    TEST_ASSERT_FALSE_MESSAGE(is_in_map, "Element shouldn't be in the map");
    is_in_map = hash_string_map_has(&m, "spring1");
    TEST_ASSERT_FALSE_MESSAGE(is_in_map, "Element shouldn't be in the map");
    is_in_map = hash_string_map_has(&m, "autumn1");
    TEST_ASSERT_FALSE_MESSAGE(is_in_map, "Element shouldn't be in the map");
    is_in_map = hash_string_map_has(&m, "umbrellau");
    TEST_ASSERT_FALSE_MESSAGE(is_in_map, "Element shouldn't be in the map");

    hash_string_map_destroy(&m);
}

void test_hash_string_map_get() {
    hash_int_map m;
    hash_string_map_init(&m);

    bool status = hash_string_map_reserve(&m, 30);

    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for map");

    bool add_status = hash_string_map_add(&m, "spring", 10) &&
        hash_string_map_add(&m, "autumn", 20) &&
        hash_string_map_add(&m, "wheel", 30) &&
        hash_string_map_add(&m, "umbrella", 40) &&
        hash_string_map_add(&m, "roomba", 50) &&
        hash_string_map_add(&m, "salsa", 100) &&
        hash_string_map_add(&m, "ball", 210) &&
        hash_string_map_add(&m, "pepper", 42);

    TEST_ASSERT_MESSAGE(add_status, "Unable to add data to map");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(8, hash_string_map_get_size(&m), "Size of"
        " a map should be 8 after adding 8 elements");

    int32_t x = -1;
    bool get_status = false;

    get_status = hash_string_map_get(&m, "spring", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(10, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "pepper", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(42, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "roomba", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(50, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "umbrella", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(40, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "wheel", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(30, x, "Incorrect retrieved value");

    // we should not get false positives
    get_status = hash_string_map_get(&m, "wheeel", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "umbrellla", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "key1", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "somedummymummy", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "rheel", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");

    hash_string_map_destroy(&m);
}

void test_hash_string_map_search_after_increased_capacity() {
    hash_int_map m;
    hash_string_map_init(&m);

    bool status = hash_string_map_reserve(&m, 20);

    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for map");
    bool add_status = hash_string_map_add(&m, "spring", 10) &&
        hash_string_map_add(&m, "autumn", 20) &&
        hash_string_map_add(&m, "wheel", 30) &&
        hash_string_map_add(&m, "umbrella", 40) &&
        hash_string_map_add(&m, "roomba", 50) &&
        hash_string_map_add(&m, "salsa", 100) &&
        hash_string_map_add(&m, "ball", 210) &&
        hash_string_map_add(&m, "pepper", 42);
    TEST_ASSERT_MESSAGE(add_status, "Unable to add data to map");

    const size_t desired_increase = 90000;
    for (size_t i = 0; i < desired_increase; ++i) {
        add_status = hash_string_map_add(&m, UUIDS[i], -999);
        TEST_ASSERT_MESSAGE(add_status, "Unable to add element to a map");
    }

    const size_t expected_size = desired_increase + 8;
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64_MESSAGE(expected_size,
        hash_string_map_get_capacity(&m), "Capacity of map is insufficient");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(expected_size,
        hash_string_map_get_size(&m), "Size of a map should increase");

    int32_t x = -1;
    bool get_status = false;

    get_status = hash_string_map_get(&m, "spring", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(10, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "pepper", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(42, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "roomba", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(50, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "umbrella", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(40, x, "Incorrect retrieved value");

    get_status = hash_string_map_get(&m, "wheel", &x);
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(30, x, "Incorrect retrieved value");

    // we should not get false positives
    get_status = hash_string_map_get(&m, "wheeel", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "umbrellla", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "key1", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "somedummymummy", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");
    get_status = hash_string_map_get(&m, "rheel", &x);
    TEST_ASSERT_FALSE_MESSAGE(get_status, "False get positive, element is not"
        " the map");

    hash_string_map_destroy(&m);
}

void test_hash_string_map_delete() {
    hash_int_map m;
    hash_string_map_init(&m);

    bool status = hash_string_map_reserve(&m, 20);

    TEST_ASSERT_MESSAGE(status, "Unable to reserve space for map");
    bool add_status = hash_string_map_add(&m, "spring", 10) &&
        hash_string_map_add(&m, "autumn", 20) &&
        hash_string_map_add(&m, "wheel", 30) &&
        hash_string_map_add(&m, "umbrella", 40) &&
        hash_string_map_add(&m, "roomba", 50) &&
        hash_string_map_add(&m, "salsa", 100) &&
        hash_string_map_add(&m, "ball", 210) &&
        hash_string_map_add(&m, "pepper", 42);
    TEST_ASSERT_MESSAGE(add_status, "Unable to add data to map");

    // should delete these items
    bool delete_status = false;
    bool search_status = false;

    delete_status = hash_string_map_delete(&m, "roomba");
    search_status = hash_string_map_has(&m, "roomba");
    TEST_ASSERT_MESSAGE(delete_status, "Delete failed - item was not found");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(7, hash_string_map_get_size(&m),
        "Size of a map should decrease after deleting an item");
    TEST_ASSERT_FALSE_MESSAGE(search_status, "The item is still in a table");

    delete_status = hash_string_map_delete(&m, "salsa");
    search_status = hash_string_map_has(&m, "salsa");
    TEST_ASSERT_MESSAGE(delete_status, "Delete failed - item was not found");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, hash_string_map_get_size(&m),
        "Size of a map should decrease after deleting an item");
    TEST_ASSERT_FALSE_MESSAGE(search_status, "The item is still in a table");

    delete_status = hash_string_map_delete(&m, "ball");
    search_status = hash_string_map_has(&m, "ball");
    TEST_ASSERT_MESSAGE(delete_status, "Delete failed - item was not found");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(5, hash_string_map_get_size(&m),
        "Size of a map should decrease after deleting an item");
    TEST_ASSERT_FALSE_MESSAGE(search_status, "The item is still in a table");

    // should not delete an item that was already deleted
    delete_status = hash_string_map_delete(&m, "roomba");
    TEST_ASSERT_FALSE_MESSAGE(delete_status, "Delete should have failed");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(5, hash_string_map_get_size(&m),
        "Size of a map should have stayed the same");

    delete_status = hash_string_map_delete(&m, "ball");
    TEST_ASSERT_FALSE_MESSAGE(delete_status, "Delete should have failed");
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(5, hash_string_map_get_size(&m),
        "Size of a map should have stayed the same");

    // adding after delete should be fine
    int32_t x;
    bool get_status = false;
    add_status = hash_string_map_add(&m, "roomba", 50);
    get_status = hash_string_map_get(&m, "roomba", &x);
    TEST_ASSERT_EQUAL_UINT64_MESSAGE(6, hash_string_map_get_size(&m),
        "Size of a map should have increased");
    TEST_ASSERT_MESSAGE(get_status, "Unable to add data after delete");
    TEST_ASSERT_MESSAGE(get_status, "Unable to get data from map after delete");
    TEST_ASSERT_EQUAL_INT32_MESSAGE(50, x, "Incorrect retrieved value");

    hash_string_map_destroy(&m);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_init_hash_string_map);
    RUN_TEST(test_hash_string_map_insert);
    RUN_TEST(test_hash_string_map_get);
    RUN_TEST(test_hash_string_map_search_after_increased_capacity);
    RUN_TEST(test_hash_string_map_delete);

    return UNITY_END();
}

