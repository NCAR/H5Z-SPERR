#include <algorithm>
#include <cassert>
#include <cmath>  // isnan()

#include <H5PLextern.h>
#include "h5zsperr_helper.h"

unsigned int C_API::h5zsperr_pack_extra_info(int rank, int is_float, int missing_val_mode, int magic)
{
  assert(rank == 3 || rank == 2);
  assert(is_float == 1 || is_float == 0);
  assert(missing_val_mode >= 0 && missing_val_mode <= 4);
  assert(magic >= 0 && magic <= 63);

  unsigned int ret = 0;

  // Bit positions 0-3 to encode the rank.
  if (rank == 2)
    ret |= 1u << 1; // Position 1
  else {
    ret |= 1u;      // Position 0
    ret |= 1u << 1; // Position 1
  }

  // Bit positions 4-5 encode data type.
  if (is_float == 1)
    ret |= 1u << 4; // Position 4

  // Bit positions 6-9 encode missing value mode.
  ret |= missing_val_mode << 6;

  // Bit positions 10-15 encode the magic number.
  ret |= magic << 10;

  return ret;
}

void C_API::h5zsperr_unpack_extra_info(unsigned int meta,
                                       int* rank,
                                       int* is_float,
                                       int* missing_val_mode,
                                       int* magic)
{
  // Extract rank from bit positions 0-3.
  unsigned pos0 = meta & 1u;
  unsigned pos1 = meta & (1u << 1);
  if (!pos0 && pos1)
    *rank = 2;
  else if (pos0 && pos1)
    *rank = 3;
  else { // error
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
            "Rank is not 2 or 3.");
  }

  // Extract data type from positions 4-5.
  unsigned pos4 = meta & (1u << 4);
  if (pos4)
    *is_float = 1;
  else
    *is_float = 0; // is_double

  // Extract missing value mode from positions 6-9.
  *missing_val_mode = 15; // 2^4 = 16
  *missing_val_mode &= meta >> 6;

  // Extract the magic number from positions 10-15.
  *magic = 63;  // 2^6 = 64
  *magic &= meta >> 10;
}

int C_API::h5zsperr_has_nan(const void* buf, size_t nelem, int is_float)
{
  assert(is_float == 1 || is_float == 0);
  if (is_float) {
    const float* p = (const float*)buf;
    return std::any_of(p, p + nelem, [](auto v) { return std::isnan(v); });
  }
  else {
    const double* p = (const double*)buf;
    return std::any_of(p, p + nelem, [](auto v) { return std::isnan(v); });
  }
}

int C_API::h5zsperr_has_large_mag(const void* buf, size_t nelem, int is_float)
{
  assert(is_float == 1 || is_float == 0);
  if (is_float) {
    const float* p = (const float*)buf;
    return std::any_of(p, p + nelem, [](auto v) { return std::abs(v) >= LARGE_MAGNITUDE_F; });
  }
  else {
    const double* p = (const double*)buf;
    return std::any_of(p, p + nelem, [](auto v) { return std::abs(v) >= LARGE_MAGNITUDE_D; });
  }
}

int C_API::h5zsperr_has_specific_f32(const void* buf, size_t nelem, float f32)
{
  const float* p = (const float*)buf;
  return std::any_of(p, p + nelem, [f32](auto v) { return v == f32; });
}

int C_API::h5zsperr_has_specific_f64(const void* buf, size_t nelem, double f64)
{
  const double* p = (const double*)buf;
  return std::any_of(p, p + nelem, [f64](auto v) { return v == f64; });
}
