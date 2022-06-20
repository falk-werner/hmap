// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Falk Werner

#include "hmap/smap.h"
#include "hmap/djb2.h"
#include <stdlib.h>
#include <string.h>

#define SMAP_INITIAL_BUCKETS 16

struct smap_entry
{
    char * key;
    void * value;
    struct smap_entry * next;
};

struct smap_bucket
{
    struct smap_entry head;
};

struct smap
{
    size_t seed;
    smap_release_fn * release_value;

    size_t entry_count;
    size_t bucket_count;
    struct smap_bucket * buckets;
};

static struct smap_bucket *
smap_getbucket(
    struct smap * map,
    char const * key)
{
    size_t hash = smap_djb2(key, map->seed);
    size_t bucket_id = hash % map->bucket_count;
    return &(map->buckets[bucket_id]);
}

static size_t smap_getthreshold(size_t bucket_count)
{
    return (7 * bucket_count) / 10;
}

static void smap_rehash(struct smap * map)
{
    // create new buckets
    size_t new_bucket_count = 2 * map->bucket_count;
    struct smap_bucket * new_buckets = malloc(new_bucket_count * sizeof(struct smap_bucket));
    for(size_t i = 0; i < new_bucket_count; i++)
    {
        struct smap_bucket * new_bucket = &(new_buckets[i]);
        new_bucket->head.next = &(new_bucket->head);
    }

    // put entries into new buckets
    for (size_t i = 0; i < map->bucket_count; i++)    
    {
        struct smap_bucket * old_bucket = &(map->buckets[i]);
        struct smap_entry * entry = old_bucket->head.next;
        while (&(old_bucket->head) != entry)
        {
            struct smap_entry * next = entry->next;

            size_t hash = smap_djb2(entry->key, map->seed);
            size_t new_bucket_id = hash % new_bucket_count;
            struct smap_bucket * new_bucket = &(new_buckets[new_bucket_id]);

            entry->next = new_bucket->head.next;
            new_bucket->head.next = entry;

            entry = next;
        }
    }

    // update map to use new buckets
    free(map->buckets);
    map->bucket_count = new_bucket_count;
    map->buckets = new_buckets;
}


struct smap * smap_create(
    size_t seed,
    smap_release_fn * release_value)
{
    struct smap * map = malloc(sizeof(struct smap));
    map->seed = seed;
    map->release_value = release_value;
    map->entry_count = 0;

    map->bucket_count = SMAP_INITIAL_BUCKETS;
    map->buckets = malloc(map->bucket_count * sizeof(struct smap_bucket));
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        map->buckets[i].head.next = &(map->buckets[i].head);
    }

    return map;
}

void smap_release(struct smap * map)
{
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        struct smap_bucket * bucket = &(map->buckets[i]);
        struct smap_entry * entry = bucket->head.next;
        struct smap_entry * end = &(bucket->head);

        while (entry != end)
        {
            struct smap_entry * next = entry->next;
            free(entry->key);
            map->release_value(entry->value);
            free(entry);

            entry = next;
        }
    }

    free(map->buckets);
    free(map);
}

void smap_add(
    struct smap * map,
    char const * key,
    void * value)
{
    if (map->entry_count > smap_getthreshold(map->bucket_count))
    {
        smap_rehash(map);
    }

    struct smap_bucket * bucket = smap_getbucket(map, key);

    bool found = false;
    struct smap_entry * entry = bucket->head.next;
    struct smap_entry * end = &(bucket->head);
    while (entry != end)
    {
        if (0 == strcmp(key, entry->key))
        {
            map->release_value(entry->value);
            entry->value = value;
            found = true;
            break;
        }

        entry = entry->next;
    }

    if (!found)
    {
        struct smap_entry * entry = malloc(sizeof(struct smap_entry));
        entry->key = strdup(key);
        entry->value = value;
        entry->next = bucket->head.next;
        bucket->head.next = entry;

        map->entry_count++;
    }
}

void const * smap_get(
    struct smap * map,
    char const * key)
{
    char const * value = NULL;

    struct smap_bucket * bucket = smap_getbucket(map, key);

    struct smap_entry * entry = bucket->head.next;
    struct smap_entry * end = &(bucket->head);
    while (entry != end)
    {
        if (0 == strcmp(key, entry->key))
        {
            value = entry->value;
            break;
        }
        entry = entry->next;
    }

    return value;
}

bool smap_contains(
    struct smap * map,
    char const * key)
{
    return (NULL != smap_get(map, key));
}

void smap_remove(
    struct smap * map,
    char const * key)
{
    struct smap_bucket * bucket = smap_getbucket(map, key);

    struct smap_entry * prev = &(bucket->head);
    struct smap_entry * entry = bucket->head.next;
    struct smap_entry * end = &(bucket->head);
    while (entry != end)
    {
        if (0 == strcmp(key, entry->key))
        {
            free(entry->key);
            map->release_value(entry->value);
            prev->next = entry->next;
            free(entry);

            map->entry_count--;
            break;
        }

        prev = entry;
        entry = entry->next;
    }
}

void smap_iter_init(
    struct smap_iter * iter,
    struct smap * map)
{
    iter->map = map;
    iter->bucket_id = -1;
    iter->entry = NULL;
}

bool smap_iter_next(
    struct smap_iter * iter)
{
    if (NULL == iter->entry)
    {
        iter->bucket_id = 0;
        iter->entry = iter->map->buckets[0].head.next;
    }
    else
    {
        struct smap_entry * bucket_end = &(iter->map->buckets[iter->bucket_id].head);
        if (iter->entry != bucket_end)
        {
            iter->entry = iter->entry->next;
        }
    }

    struct smap_entry * bucket_end = &(iter->map->buckets[iter->bucket_id].head);
    struct smap_entry * end = &(iter->map->buckets[iter->map->bucket_count - 1].head);
    while ((iter->entry != end) && (iter->entry == bucket_end))
    {
        iter->bucket_id++;
        iter->entry = iter->map->buckets[iter->bucket_id].head.next;
        bucket_end = &(iter->map->buckets[iter->bucket_id].head);
    }

    return (end != iter->entry);
}

char const * smap_iter_key(
    struct smap_iter * iter)
{
    struct smap_entry * end = &(iter->map->buckets[iter->map->bucket_count - 1].head);
    char const * key = ((NULL != iter->entry) && (iter->entry != end)) ? iter->entry->key : NULL;
    return key;
}

void const * smap_iter_value(
    struct smap_iter * iter)
{
    struct smap_entry * end = &(iter->map->buckets[iter->map->bucket_count - 1].head);
    void const * value = ((NULL != iter->entry) && (iter->entry != end)) ? iter->entry->value : NULL;
    return value;
}
