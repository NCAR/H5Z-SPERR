#include "h5zsperr_helper.h"
#include <assert.h>
#include <math.h> /* isnan() */

#include <H5PLextern.h>

unsigned int h5zsperr_pack_data_type(int rank, int is_float, int missing_val_mode)
{
  assert(rank == 3 || rank == 2);
  assert(is_float == 1 || is_float == 0);
  assert(missing_val_mode >= 0 && missing_val_mode <= 4);

  unsigned int ret = 0;

  /* Bit positions 0-3 to encode the rank. */
  if (rank == 2)
    ret |= 1u << 1; /* Position 1 */
  else {
    ret |= 1u;      /* Position 0 */
    ret |= 1u << 1; /* Position 1 */
  }

  /* Bit positions 4-5 encode data type. */
  if (is_float == 1)
    ret |= 1u << 4; /* Position 4 */

  /* Bit positions 6-9 encode missing value mode. */
  ret |= missing_val_mode << 6;

  return ret;
}

void h5zsperr_unpack_data_type(unsigned int meta,
                               int* rank,
                               int* is_float,
                               int* missing_val_mode)
{
  /* Extract rank from bit positions 0-3. */
  unsigned pos0 = meta & 1u;
  unsigned pos1 = meta & (1u << 1);
  if (!pos0 && pos1)
    *rank = 2;
  else if (pos0 && pos1)
    *rank = 3;
  else { /* error */
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
            "Rank is not 2 or 3.");
  }

  /* Extract data type from position 4-5. */
  unsigned pos4 = meta & (1u << 4);
  if (pos4)
    *is_float = 1;
  else
    *is_float = 0; /* is_double */

  /* Extract missing value mode from position 6-9. */
  *missing_val_mode = 31;
  *missing_val_mode &= meta >> 6;
}

int h5zsperr_has_nan(const void* buf, size_t nelem, int is_float)
{
  assert(is_float == 1 || is_float == 0);
  if (is_float) {
    const float* p = (const float*)buf;
    for (size_t i = 0; i < nelem; i++)
      if (isnan(p[i]))
        return 1;
  }
  else {
    const double* p = (const double*)buf;
    for (size_t i = 0; i < nelem; i++)
      if (isnan(p[i]))
        return 1;
  }

  return 0;
}

int h5zsperr_has_large_mag(const void* buf, size_t nelem, int is_float)
{
  assert(is_float == 1 || is_float == 0);
  if (is_float) {
    const float* p = (const float*)buf;
    for (size_t i = 0; i < nelem; i++)
      if (fabsf(p[i]) >= LARGE_MAGNITUDE_F)
        return 1;
  }
  else {
    const double* p = (const double*)buf;
    for (size_t i = 0; i < nelem; i++)
      if (fabs(p[i]) >= LARGE_MAGNITUDE_D)
        return 1;
  }

  return 0;
}

int h5zsperr_has_specific_f32(const void* buf, size_t nelem, float val)
{
  const float* p = (const float*)buf;
  for (size_t i = 0; i < nelem; i++)
    if (p[i] == val)
      return 1;

  return 0;
}

int h5zsperr_has_specific_f64(const void* buf, size_t nelem, double val)
{
  const double* p = (const double*)buf;
  for (size_t i = 0; i < nelem; i++)
    if (p[i] == val)
      return 1;

  return 0;
}
