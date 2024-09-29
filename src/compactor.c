#include "compactor.h"
#include <assert.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

int compactor_strategy(const void* buf, size_t bytes)
{
  assert(bytes % 8 == 0);
  const INT all0 = 0;
  const INT all1 = ~all0;
  const INT* p = (const INT*)buf;

  size_t n0 = 0, n1 = 0;
  for (size_t i = 0; i < bytes / sizeof(INT); i++) {
    INT v = p[i];
    n0 += (v == all0);
    n1 += (v == all1);
  }

  return n1 > n0;
}

size_t compactor_comp_size(const void* buf, size_t bytes)
{
  /* The compacted bitstream has the following format:
   * -- a single bit indicating the compaction strategy;
   * -- a bitstream encoding every INT;
   * -- two bits (00) indicating the finish of the compacted bitstream.
   */
  assert(bytes % 8 == 0);

  const INT all0 = 0;
  const INT all1 = ~all0;
  const INT* p = (const INT*)buf;

  size_t n0 = 0, n1 = 0;
  for (size_t i = 0; i < bytes / sizeof(INT); i++) {
    INT v = p[i];
    n0 += (v == all0);
    n1 += (v == all1);
  }
  size_t nverb = bytes / sizeof(INT) - n0 - n1;

  size_t nbits = 1;
  if (n0 >= n1) {
    nbits += n0;
    nbits += n1 * 2;
  }
  else {
    nbits += n1;
    nbits += n0 * 2;
  }
  nbits += nverb * (2 + 8 * sizeof(INT));
  nbits += 2;

  size_t nbytes = (nbits + 7) / 8;
  return nbytes;
}






