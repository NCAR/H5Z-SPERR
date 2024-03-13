#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SPERR_C_API.h>

#include <hdf5.h>
#include <H5PLextern.h>

#define H5Z_FILTER_SPERR 34000

#define H5Z_SPERR_PUSH_AND_GOTO(MAJ, MIN, RET, MSG)   \
do                                                    \
{                                                     \
    H5Epush(H5E_DEFAULT,__FILE__,_funcname_,__LINE__, \
        H5E_ERR_CLS,MAJ,MIN,MSG);                     \
    retval = RET;                                     \
    goto done;                                        \
} while(0)

static htri_t H5Z_can_apply_sperr(hid_t dcpl_id, hid_t type_id, hid_t space_id)
{
  /*
   * 	dcpl_id	  Dataset creation property list identifier
   * 	type_id	  Datatype identifier
   * 	space_id	Dataspace identifier
   */ 

  /* Get datatype class. Fail if not floats. */
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

