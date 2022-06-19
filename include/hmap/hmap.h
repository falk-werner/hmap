// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Falk Werner

#ifndef HMAP_H
#define HMAP_H

#ifndef __cplusplus
#include <stddef.h>
#include <stdbool.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Releases an item.
///
/// \param item Item to release
typedef void hmap_release_fn(void * item);

/// Computes the hash of a given key.
///
/// \param key Key to hash.
/// \return Hash value of \arg key.
typedef size_t hmap_hash_fn(void const * key);

/// Return 0 if two key are equal.
///
/// \param key Key to compare.
/// \param other_key Key to compare.
/// \return 0, if keys are equal.
typedef int hmap_equals_fn(void const * key, void const * other_key);

struct hmap;
struct hmap_bucket;
struct hmap_entry;

/// Hashmap iterator.
///
/// \note Do not use any field of this struct.
struct hmap_iter
{
    struct hmap * map;              ///< Pointer to Hashmap; do not use
    size_t bucket_id;               ///< Id of the current bucket; do not use
    struct hmap_entry * entry;      ///< Pointer to current Hashmap entry; do not use
    struct hmap_entry * end;        ///< Pointer to the last entry in the Hashmap; do not use
};

/// Sets the global seed of the Hashmap.
///
/// For security reasons, a global seed is used to randomize hash values.
/// The seed should be set once before any Hashmap is used.
///
/// \param seed Global seed of the Hashmap.
extern void hmap_seed(size_t seed);

/// Creates a new empty Hashmap.
///
/// \param hash          Hash function.
/// \param equals        Determines, whether two keys are equal.
/// \param release_key   Used to release keys.
/// \param release_value User to release values.
extern struct hmap * hmap_create(
    hmap_hash_fn * hash,
    hmap_equals_fn * equals,
    hmap_release_fn * release_key,
    hmap_release_fn * release_value
);

/// Releases a Hashmap.
///
/// \param map Pointer to the Hashmap.
extern void hmap_release(
    struct hmap * map);

/// Adds a new item to the Hashmap or updates an existing one.
///
/// \note The Hashmaps takes ownership of both, \arg key and 
///       \arg value. This is also true for updates of 
///       existing values.
///
/// \param map   Pointer to the Hashmap.
/// \param key   Key of the item to add.
/// \param value Value to add.
extern void hmap_add(
    struct hmap * map,
    void * key,
    void * value);

/// Returns a value from the Hashmap.
///
/// \param map Pointer to the Hashmap.
/// \param key Key of the item to get.
/// \return Value of the item or NULL, if the item was not found.
extern void const * hmap_get(
    struct hmap * map,
    void const * key);

/// Returns true, if the Hashmap contains an item for \arg key.
///
/// \param map Pointer to the Hashmap.
/// \param key Key of the item to find.
extern bool hmap_contains(
    struct hmap * map,
    void const * key);

/// Removes an item from the Hashmap.
///
/// \param map Pointer to the Hashmap.
/// \param key Key of the item to remove.
extern void hmap_remove(
    struct hmap * map,
    void const * key);

/// Initializes an iterator for a Hashmap.
///
/// \note The iterator is positioned before the fist element.
///       Thefore, a call to \see hmap_iter_next is needed to
///       retrieve the first key-value-pair.
/// \note The Hashmap must not be changed during iteration.
///
/// \param iter Pointer to the iterator.
/// \param map Pointer to the map.
extern void hmap_iter_init(
    struct hmap_iter * iter,
    struct hmap * map);

/// Retrieves the next key-value-pair
///
/// \note The Hashmap must not be changed during iteration.
///
/// \param iter Pointer to the iterator.
/// \return true, if there is a next key-value-pair
///         false, if there are no more items
extern bool hmap_iter_next(
        struct hmap_iter * iter);

/// Return the currently fetched value.
///
/// \note The Hashmap must not be changed during iteration.
///
/// \param iter Pointer to the iterator.
/// \return Currently fetched value or NULL, if no value is fetched.
extern void const * hmap_iter_value(
    struct hmap_iter * iter);

/// Return the currently fetched key.
///
/// \note The Hashmap must not be changed during iteration.
///
/// \param iter Pointer to the iterator.
/// \return Currently fetched key or NULL, if no key is fetched.
extern void const * hmap_iter_key(
    struct hmap_iter * iter);

#ifdef __cplusplus
}
#endif

#endif
