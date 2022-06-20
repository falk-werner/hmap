// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Falk Werner

#include "hmap/smap.h"
#include <gtest/gtest.h>


TEST(smap, create)
{
    struct smap * map = smap_create(0, &free);
    ASSERT_NE(nullptr,  map);
    smap_release(map);
}



TEST(smap, add)
{
    struct smap * map = smap_create(0, &free);

    smap_add(map, "key", strdup("value"));
    char const * value = reinterpret_cast<char const *>(smap_get(map, "key"));

    ASSERT_NE(nullptr, value);
    ASSERT_STREQ("value", value);

    smap_release(map);
}


TEST(smap, addsamekey)
{
    struct smap * map = smap_create(0, &free);

    smap_add(map, "key", strdup("value"));
    smap_add(map, "key", strdup("other"));
    char const * value = reinterpret_cast<char const *>(smap_get(map, "key"));

    ASSERT_NE(nullptr, value);
    ASSERT_STREQ("other", value);

    smap_release(map);
}


TEST(smap, getempty)
{
    struct smap * map = smap_create(0, &free);

    char const * value = reinterpret_cast<char const *>(smap_get(map, "key"));
    ASSERT_EQ(nullptr, value);

    smap_release(map);
}


TEST(smap, contains)
{
    struct smap * map = smap_create(0, &free);

    bool is_contained = smap_contains(map, "key");
    ASSERT_FALSE(is_contained);

    smap_add(map, "key", strdup("value"));
    is_contained = smap_contains(map, "key");
    ASSERT_TRUE(is_contained);

    smap_release(map);
}


TEST(smap, remove)
{
    struct smap * map = smap_create(0, &free);

    smap_add(map, "key", strdup("value"));
    bool is_contained = smap_contains(map, "key");
    ASSERT_TRUE(is_contained);

    smap_remove(map, "key");
    is_contained = smap_contains(map, "key");
    ASSERT_FALSE(is_contained);

    smap_release(map);
}

TEST(smap, rehash)
{
    struct smap * map = smap_create(0, &free);
    size_t count = 128;

    // add some item to hashmap to trigger rehash
    for(int i = 0; i < count; i++)
    {
        char buffer[10];
        snprintf(buffer, 10, "%d", i);

        char * key = buffer;
        char * value = strdup(buffer);
        smap_add(map, key, value);        
    }

    // test if values are contained
    for(int i = 0; i < count; i++)
    {
        char key[10];
        snprintf(key, 10, "%d", i);

        bool is_contained = smap_contains(map, key);
        ASSERT_TRUE(is_contained);
    }

    smap_release(map);
}

/*
TEST(smap, iter_empty)
{
    struct smap * map = smap_create(0, &free);

    struct smap_iter iter;
    smap_iter_init(&iter, map);

    bool has_value = smap_iter_next(&iter);
    ASSERT_FALSE(has_value);

    smap_release(map);
}


TEST(smap, iter_some)
{
    struct smap * map = smap_create(0, &free);

    smap_add(map, "1", strdup("A"));
    smap_add(map, "2", strdup("B"));
    smap_add(map, "3", strdup("C"));

    struct hmap_iter iter;
    smap_iter_init(&iter, map);
    
    size_t count = 0;
    while (smap_iter_next(&iter))
    {
        count++;
        char const * key = reinterpret_cast<char const*>(smap_iter_key(&iter));
        char const * value = reinterpret_cast<char const*>(smap_iter_value(&iter));

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

    smap_release(map);
}
*/