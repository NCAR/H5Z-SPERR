#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SPERR_C_API.h>

#include <hdf5.h>
#include <H5PLextern.h>

#define H5Z_FILTER_SPERR 34000

static htri_t H5Z_can_apply_sperr(hid_t dcpl_id, hid_t type_id, hid_t space_id)
{
  /*
   * 	dcpl_id	  Dataset creation property list identifier
   * 	type_id	  Datatype identifier
   * 	space_id	Dataspace identifier
   */ 

  /* Get datatype class. Fail if not floats. */
  if (H5Tget_class(type_id) != H5T_FLOAT) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADTYPE,
            "bad data type. Only floats are supported in H5Z-SPERR");
    return 0;
  }

  /* Get the dataspace rank. Fail if not 2 or 3. */
  int ndims = H5Sget_simple_extent_ndims(space_id);
  if (ndims < 2 || ndims > 3) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADTYPE,
            "bad ranks. Only rank==2 or rank==3 are supported in H5Z-SPERR");
    return 0;
  }

  /* Chunks have to be 2D or 3D as well, and to be conservative, we also check chunk sizes. */
  hsize_t chunks[3];
  ndims = H5Pget_chunk(dcpl_id, 3, chunks);
  if (ndims < 2 || ndims > 3) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADTYPE,
            "bad chunk ranks. Only rank==2 or rank==3 are supported in H5Z-SPERR");
    return 0;
  }

  bool bad_chunk = false;
  for (int i = 0; i < ndims; i++) {
    if (chunks[i] < 9)
      bad_chunk = true;
  }
  if (bad_chunk) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADTYPE,
            "bad chunk dimensions. (may relax this requirement in the future)");
    return 0;
  }

  return 1;
}
static herr_t H5Z_set_local_sperr(hid_t dcpl_id, hid_t type_id, hid_t space_id)
{
  return 0;
}
static size_t H5Z_filter_sperr(unsigned int flags, size_t cd_nelmts,
                               const unsigned int cd_values[], size_t nbytes,
                               size_t *buf_size, void **buf)
{
  unsigned char cksum[16];

  /* Read */
  if (flags & H5Z_FLAG_REVERSE) {
#if 0
    assert(nbytes>=16);
    calc_md5(nbytes-16, *buf, cksum);

    /* Compare */
    if (memcmp(cksum, (unsigned char*)(*buf)+nbytes-16, 16ul))
      return 0; /*fail*/

    /* Strip off checksum */
    return nbytes-16;
#endif
    return nbytes;
  } 
  else { /* Write */
#if 0
    calc_md5(nbytes, *buf, cksum);

    /* Increase buffer size if necessary */
    if (nbytes+16>*buf_size) {
      *buf_size = nbytes + 16;
      *buf = realloc(*buf, *buf_size);
    }

    /* Append checksum */
    memcpy((unsigned char*)(*buf)+nbytes, cksum, 16ul);
    return nbytes+16;
#endif
    return nbytes;
  }
}

const H5Z_class2_t H5Z_SPERR_class_t = {
                   H5Z_CLASS_T_VERS,    /* H5Z_class_t version */
                   H5Z_FILTER_SPERR,    /* Filter id number    */
                   1,                   /* encoder_present flag (set to true) */
                   1,                   /* decoder_present flag (set to true) */
                   "h5z-sperr",         /* Filter name for debugging  */
                   H5Z_can_apply_sperr, /* The "can apply" callback   */
                   H5Z_set_local_sperr, /* The "set local" callback   */
                   H5Z_filter_sperr};   /* The actual filter function */

const void* H5PLget_plugin_info()
{
  return &H5Z_SPERR_class_t;
}

H5PL_type_t H5PLget_plugin_type()
{
  return H5PL_TYPE_FILTER;
}

