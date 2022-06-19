// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Falk Werner

#include "hmap/hmap.h"
#include <gtest/gtest.h>

namespace
{

size_t string_hash(void const * item)
{
    char const * value = reinterpret_cast<char const *>(item);
    size_t result = 0;

    for (size_t i = 0; '\0' != value[i]; i++)
    {
        result += (value[i] && 0xff);
    }

    return result;
}

int string_equals(void const * value, void const * other)
{
    return strcmp(reinterpret_cast<char const *>(value), reinterpret_cast<char const *>(other));
}


}


TEST(hmap, create)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);
    ASSERT_NE(nullptr,  map);
    hmap_release(map);
}



TEST(hmap, add)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    hmap_add(map, strdup("key"), strdup("value"));
    char const * value = reinterpret_cast<char const *>(hmap_get(map, reinterpret_cast<void const*>("key")));

    ASSERT_NE(nullptr, value);
    ASSERT_EQ(0, strcmp("value", value));

    hmap_release(map);
}


TEST(hmap, addsamekey)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    hmap_add(map, strdup("key"), strdup("value"));
    hmap_add(map, strdup("key"), strdup("other"));
    char const * value = reinterpret_cast<char const *>(hmap_get(map, "key"));

    ASSERT_NE(nullptr, value);
    ASSERT_EQ(0, strcmp("other", value));

    hmap_release(map);
}


TEST(hmap, getempty)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    char const * value = reinterpret_cast<char const *>(hmap_get(map, "key"));
    ASSERT_EQ(nullptr, value);

    hmap_release(map);
}


TEST(hmap, contains)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    bool is_contained = hmap_contains(map, "key");
    ASSERT_FALSE(is_contained);

    hmap_add(map, strdup("key"), strdup("value"));
    is_contained = hmap_contains(map, "key");
    ASSERT_TRUE(is_contained);

    hmap_release(map);
}


TEST(hmap, remove)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    hmap_add(map, strdup("key"), strdup("value"));
    bool is_contained = hmap_contains(map, "key");
    ASSERT_TRUE(is_contained);

    hmap_remove(map, "key");
    is_contained = hmap_contains(map, "key");
    ASSERT_FALSE(is_contained);

    hmap_release(map);
}

TEST(hmap, rehash)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);
    size_t count = 128;

    // add some item to hashmap to trigger rehash
    for(int i = 0; i < count; i++)
    {
        char buffer[10];
        snprintf(buffer, 10, "%d", i);

        char * key = strdup(buffer);
        char * value = strdup(buffer);
        hmap_add(map, key, value);        
    }

    // test if values are contained
    for(int i = 0; i < count; i++)
    {
        char key[10];
        snprintf(key, 10, "%d", i);

        bool is_contained = hmap_contains(map, key);
        ASSERT_TRUE(is_contained);
    }

    hmap_release(map);
}

TEST(hmap, iter_empty)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    struct hmap_iter iter;
    hmap_iter_init(&iter, map);

    bool has_value = hmap_iter_next(&iter);
    ASSERT_FALSE(has_value);

    hmap_release(map);
}


TEST(hmap, iter_some)
{
    struct hmap * map = hmap_create(&string_hash, &string_equals, &free, &free);

    hmap_add(map, strdup("1"), strdup("A"));
    hmap_add(map, strdup("2"), strdup("B"));
    hmap_add(map, strdup("3"), strdup("C"));

    struct hmap_iter iter;
    hmap_iter_init(&iter, map);
    
    size_t count = 0;
    while (hmap_iter_next(&iter))
    {
        count++;
        char const * key = reinterpret_cast<char const*>(hmap_iter_key(&iter));
        char const * value = reinterpret_cast<char const*>(hmap_iter_value(&iter));

        ASSERT_NE(nullptr, key);
        ASSERT_NE(nullptr, value);

        if (0 == strcmp(key, "1"))
        {
            ASSERT_EQ(0, strcmp(value, "A"));
        }
        else if (0 == strcmp(key, "2"))
        {
            ASSERT_EQ(0, strcmp(value, "B"));
        }
        else if (0 == strcmp(key, "3"))
        {
            ASSERT_EQ(0, strcmp(value, "C"));
        }
        else
        {
            FAIL() << "unknown value";
        }        
    }

    ASSERT_EQ(3, count);

    hmap_release(map);
}
