// SPDX-License-Identifier: MIT
// Copyright (c) 2022 Falk Werner

#ifndef SMAP_H
#define SMAP_H

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
/// \param item Item to release.
typedef void smap_release_fn(void * item);

struct smap;
struct smap_bucket;
struct smap_entry;

/// Hashmap iterator.
///
/// \note Do note use any field of this struct.
struct smap_iter
{
    struct smap * map;              ///< Pointer to Hashmap; do not use
    size_t bucket_id;               ///< Id of the current bucket; do not use
    struct smap_entry * entry;      ///< Pointer to the current Hashmap entry; do not use
};

/// Creates a new Hashmap with string keys.
///
/// \param seed          Seed used for hash randomization.
/// \param release_value Used to release values.
/// \return newly creates Hashmap.
extern struct smap * smap_create(
    size_t seed,
    smap_release_fn * release_value);

/// Releases a Hashmap.
///
/// \param map Pointer to Hashmap.
extern void smap_release(struct smap * map);

/// Adds or updates a value.
///
/// \param map Pointer to Hashmap.
/// \param key Key of the value.
/// \param value value to add or update.
extern void smap_add(
    struct smap * map,
    char const * key,
    void * value);

/// Return the value of a given key.
///
/// \param map Pointer to Hashmap.
/// \param key Key of the value to get.
/// \return Value assiciated with \arg key or NULL, if key not found.
extern void const * smap_get(
    struct smap * map,
    char const * key);

/// Returns true, if the Hashmap contains \arg key.
///
/// \param map Pointer to Hashmap.
/// \param key Key to test.
/// \return True, if \arg key is contained in the Hashmap, otherwise false.
extern bool smap_contains(
    struct smap * map,
    char const * key);

/// Removes an item from the Hashmap.
///
/// \param map Pointer to the Hashmap.
/// \param key Key of the item to remove.
extern void smap_remove(
    struct smap * map,
    char const * key);

/// Initialized an iterator for a given Hashmap.
///
/// \note The iterator is positioned before the first element.
///       Therefore, a call to \see smap_inter_next is needed to
///       retrieve the first item.
///
/// \note The Hashmap must not be changed during iteration.
///
/// \param iter Pointer to the iterator.
/// \param map Pointer to the Hashmap to iterate.
extern void smap_iter_init(
    struct smap_iter * iter,
    struct smap * map);

/// Retrieves the next item of the Hashmap.
///
/// \note The Hashmap must not be changes during iteration.
///
/// \param iter Pointer to the iterator.
/// \return True, if the next item is fetched successfully, otherwise false.
extern bool smap_iter_next(
    struct smap_iter * iter);

/// Returns the currently fetched key.
///
/// \note The Hashmap must not be changes during iteration.
///
/// \param iter Pointer to the iterator.
/// \return Currently fetched key or NULL, if no key is fetched. 
extern char const * smap_iter_key(
    struct smap_iter * iter);

/// Returns the currently fetched value.
///
/// \note The Hashmap must not be changes during iteration.
///
/// \param iter Pointer to the iterator.
/// \return Currently fetched value or NULL, if no value is fetched. 
extern void const * smap_iter_value(
    struct smap_iter * iter);

#ifdef __cplusplus
}
#endif


#endif
