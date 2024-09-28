/*
 * This is a mimic of the Bitstream class in SPERR:
 * https://github.com/NCAR/SPERR/blob/main/include/Bitstream.h
 *
 * The most significant difference is that bitstream here doesn't manage
 * any memory; it reads a bit sequence from a user-provided memory buffer,
 * or writes a bit sequence to a user-provided memory buffer.
 * 
 * The "object" is named `h5z_bitstream` and all functions operating on it
 * are named with a prefix `icecream`.
 */

#ifndef H5Z_BITSTREAM_H
#define H5Z_BITSTREAM_H

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

struct h5z_bitstream{
  uint64_t* begin;  /* begin of the stream */
  uint64_t* ptr;    /* pointer to the next word to be read/written */
  uint64_t  buffer; /* incoming/outgoing bits */
  int       bits;   /* number of buffered bits (0 <= bits < 64) */
};
typedef struct h5z_bitstream icecream;

/*
 * Specify a bitstream to use memory provided by users.
 * NOTE: the memory length (in bytes) does not need to be a multiplier of 8,
 * however, reading from or writing to the last incomplete word (i.e., the
 * last y bytes where y < 8) will result in memory errors.
 * For example, given a memory buffer of 20 bytes, it is only safe
 * to read or write to the first 16 bytes, or 16 x 8 = 128 bits.
 */
void icecream_use_mem(icecream* s, void* mem, size_t bytes);

/* Position the bitstream for reading or writing at the beginning. */
void icecream_rewind(icecream* s);

/* Read a bit. Please don't read beyond the end of the stream. */
int icecream_rbit(icecream* s);

/* Write a bit (0 or 1). Please don't write beyond the end of the stream. */
void icecream_wbit(icecream* s, int bit);

/* Return the bit offset to the next bit to be read. */
size_t icecream_rtell(icecream* s);

/* Return the bit offset to the next bit to be written. */
size_t icecream_wtell(icecream* s);

/* Write any remaining buffered bits and align stream on next word boundary. */
void icecream_flush(icecream* s);

#endif
