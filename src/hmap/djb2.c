#include "hmap/djb2.h"

size_t smap_djb2(char const * key, size_t seed)
{
    size_t hash = 5381 + seed;

    while ('\0' != key[0])
    {
        hash = (hash * 33) ^ key[0];
        key++;
    }

    return hash;
}
