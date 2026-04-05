#include <stdbool.h>

#include "ak/assert.h"
#include "ak/bitset.h"
#include "ak/types.h"

// We use a single set() with a bool value to handle both setting and clearing.
// This avoids branching (if/else) by using the following bitmasking logic:
//
// Example (position = 3):
// 1. Create a bitmask by shifting 1 into position: (1U << 3) -> 00001000
// 2. Flip the mask: ~(00001000) -> 11110111
// 3. AND the original with the mask to zero out the target bit.
// 4. OR that result with the new value shifted to the target position.

//
// u8
//

void bitset8_set(u8* bitset, u8 position, bool value)
{
    ASSERT(bitset && position < 8);
    *bitset = (*bitset & ~(u8)(1U << position)) | (u8)((u8)value << position);
}

bool bitset8_get(u8 bitset, u8 position)
{
    ASSERT(position < 8);
    return (bool)((bitset >> position) & 1U);
}

//
// u64
//

void bitset64_set(u64* bitset, u8 position, bool value)
{
    ASSERT(bitset && position < 64);
    *bitset = (*bitset & ~(1ULL << position)) | ((u64)value << position);
}

bool bitset64_get(u64 bitset, u8 position)
{
    ASSERT(position < 64);
    return (bool)((bitset >> position) & 1ULL);
}
