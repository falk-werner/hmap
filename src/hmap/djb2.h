#ifndef SMAP_DJB2_H
#define SMAP_DJB2_H

#ifndef __cplusplus
#include <stddef.h>
#else
#include <cstddef>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/// Returns a hash for string values.
///
/// Provides a slightly modified version of djb2 has function
/// by Dan Bernstein (see http://www.cse.yorku.ca/~oz/hash.html).
/// The modification is made to support a seed for hash randomization.
///
/// \param key Key to hash.
/// \param seed Seed for hash randomization.
/// \return Hash value of \arg key.
extern size_t smap_djb2(char const * key, size_t seed);

#ifdef __cplusplus
}
#endif

#endif
