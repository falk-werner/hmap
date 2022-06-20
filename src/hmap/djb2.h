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

// djb2 by Dan Bernstein is used to hash strings
// (see http://www.cse.yorku.ca/~oz/hash.html)
extern size_t smap_djb2(char const * key, size_t seed);

#ifdef __cplusplus
}
#endif

#endif
