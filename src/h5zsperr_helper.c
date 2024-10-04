#include "h5zsperr_helper.h"
#include <assert.h>

#include <H5PLextern.h>

unsigned int h5zsperr_pack_data_type(int rank, int dtype)
{
  unsigned int ret = 0;

  /*
   * Bit position 0-3 to encode the rank.
   * Since this function is called from `set_local()`, it should always be 2 or 3.
   */
  if (rank == 2) {
    ret |= 1u << 1; /* Position 1 */
  }
  else {
    assert(rank == 3);
    ret |= 1u;      /* Position 0 */
    ret |= 1u << 1; /* Position 1 */
  }

  /*
   * Bit position 4-7 encode data type.
   * Only float (1) and double (0) are supported right now.
   */
  if (dtype == 1)   /* is_float   */
    ret |= 1u << 4; /* Position 4 */
  else
    assert(dtype == 0);

  return ret;
}

void h5zsperr_unpack_data_type(unsigned int meta, int* rank, int* dtype)
{
  /*
   * Extract rank from bit positions 0-3.
   */
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

  /*
   * Extract data type from position 4-7.
   * Only float and double are supported right now.
   */
  unsigned pos4 = meta & (1u << 4);
  if (pos4)
    *dtype = 1; /* is_float  */
  else
    *dtype = 0; /* is_double */
}
