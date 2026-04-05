#include "ak/types.h"

// bitset functions to simlify setting and getting individual bits.
//
// Instead of 0s and 1s we use bool.

void bitset8_set(u8* bitset, u8 position, bool value);
bool bitset8_get(u8 bitset, u8 position);

void bitset64_set(u64* bitset, u8 position, bool value);
bool bitset64_get(u64 bitset, u8 position);
