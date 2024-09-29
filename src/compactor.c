#include "compactor.h"

int compactor_strategy(const void* buf, size_t bytes)
{
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
