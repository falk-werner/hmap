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

typedef void smap_release_fn(void * item);

struct smap;

extern struct smap * smap_create(
    size_t seed,
    smap_release_fn * release_value);

extern void smap_release(struct smap * map);

extern void smap_add(
    struct smap * map,
    char const * key,
    void * value);

extern void const * smap_get(
    struct smap * map,
    char const * key);

extern bool smap_contains(
    struct smap * map,
    char const * key);

extern void smap_remove(
    struct smap * map,
    char const * key);



#ifdef __cplusplus
}
#endif


#endif
