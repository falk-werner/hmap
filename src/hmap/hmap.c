// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Falk Werner

#include "hmap/hmap.h"
#include <stdlib.h>

#define HMAP_INITIAL_BUCKETS 16

struct hmap_entry
{
    void * key;
    void * value;
    struct hmap_entry * next;
};

struct hmap_bucket
{
    struct hmap_entry head;
};

struct hmap
{
    size_t seed;
    hmap_hash_fn * hash;
    hmap_equals_fn * equals;
    hmap_release_fn * release_key;
    hmap_release_fn * releae_value;

    size_t entry_count;
    size_t bucket_count;
    struct hmap_bucket * buckets;
};


static struct hmap_bucket * hmap_getbucket(
    struct hmap * map,
    void const * key)
{
    size_t hash = map->hash(key, map->seed);
    size_t bucket_id = hash % map->bucket_count;
    return &(map->buckets[bucket_id]);
}


static size_t hmap_getthreshold(size_t bucket_count)
{
    return (7 * bucket_count) / 10;
}

static void hmap_rehash(struct hmap * map)
{
    // create new buckets
    size_t new_bucket_count = 2 * map->bucket_count;
    struct hmap_bucket * new_buckets = malloc(new_bucket_count * sizeof(struct hmap_bucket));
    for(size_t i = 0; i < new_bucket_count; i++)
    {
        struct hmap_bucket * new_bucket = &(new_buckets[i]);
        new_bucket->head.next = &(new_bucket->head);
    }

    // put entries into new buckets
    for (size_t i = 0; i < map->bucket_count; i++)    
    {
        struct hmap_bucket * old_bucket = &(map->buckets[i]);
        struct hmap_entry * entry = old_bucket->head.next;
        while (&(old_bucket->head) != entry)
        {
            struct hmap_entry * next = entry->next;

            size_t hash = map->hash(entry->key, map->seed);
            size_t new_bucket_id = hash % new_bucket_count;
            struct hmap_bucket * new_bucket = &(new_buckets[new_bucket_id]);

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

struct hmap * hmap_create(
    size_t seed,
    hmap_hash_fn * hash,
    hmap_equals_fn * equals,
    hmap_release_fn * release_key,
    hmap_release_fn * release_value
)
{
    struct hmap * map = malloc(sizeof(struct hmap));
    map->seed = seed;
    map->hash = hash;
    map->equals = equals;
    map->release_key = release_key;
    map->releae_value = release_value;
    map->entry_count = 0;

    map->bucket_count = HMAP_INITIAL_BUCKETS;
    map->buckets = malloc(map->bucket_count * sizeof(struct hmap_bucket));
    for (size_t i = 0; i < HMAP_INITIAL_BUCKETS; i++)
    {
        map->buckets[i].head.next = &(map->buckets[i].head);
    }

    return map;
}

void hmap_release(
    struct hmap * map)
{
    for (size_t i = 0; i < map->bucket_count; i++)
    {
        struct hmap_bucket * bucket = &(map->buckets[i]);
        struct hmap_entry * entry = bucket->head.next;

        while (&(bucket->head) != entry)
        {
            struct hmap_entry * next = entry->next;
            map->release_key(entry->key);
            map->releae_value(entry->value);
            free(entry);

            entry = next;
        }
    }

    free(map->buckets);
    free(map);
}

void hmap_add(
    struct hmap * map,
    void * key,
    void * value)
{
    if (map->entry_count > hmap_getthreshold(map->bucket_count))
    {
        hmap_rehash(map);
    }

    struct hmap_bucket * bucket = hmap_getbucket(map, key);

    // try to find previously added key
    bool found = false;
    struct hmap_entry * entry = bucket->head.next;
    while (&(bucket->head) != entry)
    {
        if (0 == map->equals(key, entry->key))
        {
            map->release_key(entry->key);
            map->releae_value(entry->value);
            entry->key = key;
            entry->value = value;
            found = true;
            break;
        }
        entry = entry->next;
    }

    if (!found)
    {
        struct hmap_entry * entry = malloc(sizeof(struct hmap_entry));
        entry->key = key;
        entry->value = value;
        entry->next = bucket->head.next;

        bucket->head.next = entry;
        map->entry_count++;
    }

}

void const * hmap_get(
    struct hmap * map,
    void const * key)
{
    void * value = NULL;

    struct hmap_bucket * bucket = hmap_getbucket(map, key);

    struct hmap_entry * entry = bucket->head.next;
    while (&(bucket->head) != entry)
    {
        if (0 == map->equals(key, entry->key))
        {
            value = entry->value;
            break;
        }
        entry = entry->next;
    }

    return value;
}

bool hmap_contains(
    struct hmap * map,
    void const * key)
{
    return (NULL != hmap_get(map, key));
}

void hmap_remove(
    struct hmap * map,
    void const * key)
{
    struct hmap_bucket * bucket = hmap_getbucket(map, key);

    struct hmap_entry * prev = &(bucket->head);
    struct hmap_entry * entry = bucket->head.next;
    while (&(bucket->head) != entry)
    {
        if (0 == map->equals(key, entry->key))
        {
            map->release_key(entry->key);
            map->releae_value(entry->value);
            prev->next = entry->next;
            free(entry);

            map->entry_count--;
            break;
        }
        prev = prev->next;
        entry = entry->next;
    }
}

void hmap_iter_init(
    struct hmap_iter * iter,
    struct hmap * map)
{
    size_t last_bucket_id = map->bucket_count - 1;

    iter->map = map;
    iter->bucket_id = -1;
    iter->entry = NULL;
    iter->end = &(map->buckets[last_bucket_id].head);
}

bool hmap_iter_next(
        struct hmap_iter * iter)
{
    if (NULL == iter->entry)
    {
        iter->bucket_id = 0;
        iter->entry = iter->map->buckets[0].head.next;
    }
    else
    {
        struct hmap_entry * bucket_end = &(iter->map->buckets[iter->bucket_id].head);

        if (bucket_end != iter->entry)
        {
            iter->entry = iter->entry->next;
        }
    }

    struct hmap_entry * bucket_end = &(iter->map->buckets[iter->bucket_id].head);
    while ((iter->end != iter->entry) && (iter->entry == bucket_end))
    {
        iter->bucket_id++;
        iter->entry = iter->map->buckets[iter->bucket_id].head.next;
        bucket_end = &(iter->map->buckets[iter->bucket_id].head);        
    }

    return (iter->end != iter->entry);
}

void const * hmap_iter_value(
    struct hmap_iter * iter)
{
    void const * value = ((NULL != iter->entry) && (iter->end != iter->entry)) ? iter->entry->value : NULL;
    return value;
}

void const * hmap_iter_key(
    struct hmap_iter * iter)
{
    void const * key = ((NULL != iter->entry) && (iter->end != iter->entry)) ? iter->entry->key : NULL;
    return key;
}
