/* This is a set of functions that compact a bitmask.
 * In the intended use case, a bitmask is produced by masking all
 * "missing values" or "fill values" in a model output with zero's,
 * whereas the locations with valid data points are marked with one's.
 * However, this compactor is likely to be effective with any bit patterns
 * that have lots of consecutive 0's or 1's.
 *
 * The bitmask compactor works in the following way:
 * 1. Assume that we use 32-bit ints; the compactor encodes 32 bits at a time.
 * 2. Every incoming int is encoded in one of three ways:
 *    2.1. For an int with all 0's, use a single 0 bit.
 *    2.2. For an int with all 1's, use two bits: 10.
 *    2.3. For all other ints, use 34 bits: two bits 11 then followed by the
 *         verbose presentation of the 32-bit int.
 * 3. The encoding procedure finishes with two bits: 00.
 * 4. Obviously, it's the most economical to use a single 0 bit to present
 *    the most frequent pattern (all 0's or all 1's). The encoder thus does
 *    a test at the beginning and records the test result.
 */

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

/* Change this typedef to use different width. */
typedef uint32_t INT;

/*
 * Given a chunk of memory `buf` with length `len` in bytes, return whether
 * all 0's or all 1's is more frequent.
 */
int bitcpt_freq(const void* buf, size_t len);
