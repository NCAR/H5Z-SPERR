#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <openssl/md5.h>

#include "hdf5.h"
#include <H5PLextern.h>

#define H5Z_FILTER_MD5 33000

void calc_md5(size_t len, const void* buf,  /* Input */
              unsigned char* sum)           /* Output */
{
  /* Create an MD5 context */
  MD5_CTX md5_context;
  MD5_Init(&md5_context);

  /* Update the context with the data to be hashed */
  MD5_Update(&md5_context, buf, len);

  /* Finalize the context and get the hash */
  MD5_Final(sum , &md5_context);
}

H5PL_type_t H5PLget_plugin_type(void) {return H5PL_TYPE_FILTER;}

static htri_t H5Z_can_apply_md5(hid_t dcpl_id, hid_t type_id, hid_t space_id)
{
  return 1;
}
static herr_t H5Z_set_local_md5(hid_t dcpl_id, hid_t type_id, hid_t space_id)
{
  // unsigned cd_in[2] = {4, 5};
  // H5Pmodify_filter(dcpl_id, H5Z_FILTER_MD5, H5Z_FLAG_MANDATORY, 2, cd_in);
  return 0;
}
static size_t H5Z_filter_md5(unsigned int flags, size_t cd_nelmts,
                             const unsigned int cd_values[], size_t nbytes,
                             size_t *buf_size, void **buf)
{
  unsigned char cksum[16];

  /* Read */
  if (flags & H5Z_FLAG_REVERSE) {
    assert(nbytes>=16);
    calc_md5(nbytes-16, *buf, cksum);

    /* Compare */
    if (memcmp(cksum, (unsigned char*)(*buf)+nbytes-16, 16ul))
      return 0; /*fail*/

    /* Strip off checksum */
    return nbytes-16;

  } 
  else { /* Write */
    calc_md5(nbytes, *buf, cksum);

    /* Increase buffer size if necessary */
    if (nbytes+16>*buf_size) {
      *buf_size = nbytes + 16;
      *buf = realloc(*buf, *buf_size);
    }

    /* Append checksum */
    memcpy((unsigned char*)(*buf)+nbytes, cksum, 16ul);
    return nbytes+16;
  }
}

const H5Z_class2_t H5Z_MD5[1] = {{
                    H5Z_CLASS_T_VERS,    /* H5Z_class_t version */
                    H5Z_FILTER_MD5,      /* Filter id number    */
                    1,                   /* Assume encoder present: check before registering */
                    1,                   /* decoder_present flag (set to true) */
                    "h5z-md5",           /* Filter name for debugging */
                    H5Z_can_apply_md5,   /* The "can apply" callback     */
                    H5Z_set_local_md5,   /* The "set local" callback     */
                    H5Z_filter_md5,      /* The actual filter function  */
}};

const void* H5PLget_plugin_info(void) {return H5Z_MD5;}

