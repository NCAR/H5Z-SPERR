#include "h5z-bitstream.h"

void icecream_use_mem(icecream* s, void* mem, size_t bytes) {
  s->begin = (uint64_t*)mem;
  icecream_rewind(s);
}

void icecream_rewind(icecream* s)
{
  s->ptr = s->begin;
  s->buffer = 0;
  s->bits = 0;
}

