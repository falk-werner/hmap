# Changelog

## v2.0.0

- **[Fix/Breaking Change]**: Fixed hash randomization (by moving `seed` into `hmap_hash_fn`)
  - removed `hmap_setseed`
  - added `seed` as parameter to `hmap_create`
  - added `seed` as paramter to  `hmap_hash_fn`
- **[Feature]**: Added smap (hash map using string keys)

## v1.0.0

- Initial release