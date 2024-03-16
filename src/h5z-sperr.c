#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SPERR_C_API.h>

#include <H5PLextern.h>
#include <hdf5.h>

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

/*
 * Pack three types of information into an `unsigned int`.
 * It returns 0 on error, and non-zero on success.
 */
static unsigned int H5Z_SPERR_pack_meta(int rank,      /* Input */
                                        int dtype,     /* Input */
                                        int comp_mode) /* Input */
{
  unsigned int ret = 0;

  /*
   * Bit position 0-3 to encode the rank.
   * Only support 2, 3 right now.
   */
  if (rank == 2) {
    ret |= 1u << 1; /* Position 1 */
  }
  else if (rank == 3) {
    ret |= 1u;      /* Position 0 */
    ret |= 1u << 1; /* Position 1 */
  }
  else {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
            "Only 2D or 3D spaces are supported.");
    return 0;
  }

  /*
   * Bit position 4-5 encode data type.
   * Only float (1) and double (0) are supported right now.
   */
  if (dtype == 1)       /* is_float   */
    ret |= 1u << 4;     /* Position 4 */
  else if (dtype > 1) { /* error */
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADTYPE,
            "Only 32-bit or 64-bit floating point values are supported.");
    return 0;
  }

  /*
   * Bit position 6-8 encode compression mode.
   * Only BPP (1), PSNR (2), and PWE (3) modes are supported right now.
   */
  switch (comp_mode) {
    case 1:
      ret |= 1u << 6;
      break;
    case 2:
      ret |= 1u << 7;
      break;
    case 3:
      ret |= 1u << 6;
      ret |= 1u << 7;
      break;
    default:
      H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
              "Unsupported compression mode.");
      return 0;
  }

  return ret;
}

/*
 * Unpack three types of information from an `unsigned int`.
 * It returns non-zero on error, and zero on success.
 */
static int H5Z_SPERR_unpack_meta(unsigned int meta, /* Input  */
                                 int* rank,         /* Output */
                                 int* dtype,        /* Output */
                                 int* comp_mode)    /* Output */
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
    return 1;
  }

  /*
   * Extract data type from position 4-5.
   * Only float and double are supported right now.
   */
  unsigned pos4 = meta & (1u << 4);
  if (pos4)
    *dtype = 1; /* is_float  */
  else
    *dtype = 0; /* is_double */

  /*
   * Extract compression mode from position 6-8.
   */
  unsigned pos6 = meta & 1u << 6;
  unsigned pos7 = meta & 1u << 7;
  if (pos6 && !pos7)
    *comp_mode = 1;
  else if (!pos6 && pos7)
    *comp_mode = 2;
  else if (pos6 && pos7)
    *comp_mode = 3;
  else { /* error */
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
            "Compression type not supported.");
    return 1;
  }

  return 0;
}

static herr_t H5Z_set_local_sperr(hid_t dcpl_id, hid_t type_id, hid_t space_id)
{
  /*
   * 	dcpl_id	  Dataset creation property list identifier
   * 	type_id	  Datatype identifier
   * 	space_id	Dataspace identifier
   */

  /* Get the dataspace rank. It must be 2 or 3, since it passed the `can_apply` function. */
  int rank = H5Sget_simple_extent_ndims(space_id);

  /* Get the datatype size. It must be 4 or 8, since the float type is verified by `can_apply`. */
  int is_float = 1;
  if (H5Tget_size(type_id) == 8)
    is_float = 0; /* !is_float i.e. double */

  /* Get chunk sizes. */
  hsize_t chunks[3];
  int ndims = H5Pget_chunk(dcpl_id, 3, chunks);
  if (ndims != rank) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADSIZE,
            "Somehow the chunk rank is different from two queries ??");
    return -1;
  }

  /* Get the user-specified compression mode and quality. */
  size_t user_cd_nelem = 16, nchar = 16;
  unsigned int user_cd_values[user_cd_nelem], flags;
  char name[nchar];
  herr_t status =
      H5Pget_filter_by_id(dcpl_id, H5Z_FILTER_SPERR, &flags, &user_cd_nelem, user_cd_values, nchar,
                          name, user_cd_values + user_cd_nelem - 1);
  if (user_cd_nelem != 3) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADSIZE,
            "User cd_values[] isn't 3 elements ??");
    return -1;
  }
  int mode = user_cd_values[0];
  double quality = 0.0;
  memcpy(&quality, &user_cd_values[1], sizeof(quality));
  if (quality < 0.0) {
    H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
            "User specified quality is negative ??");
    return -1;
  }

  /*
   * Assemble the meta info (cd_values[5] or cd_values[6]) to be stored.
   * [0]  : 2D/3D, float/double, compression mode.
   * [1-2]: As a double, compression quality.
   * [3-4]: (dimx, dimy) in 2D cases.
   * [3-5]: (dimx, dimy, dimz) in 3D cases.
   */
  unsigned int cd_values[6] = {0, 0, 0, 0, 0, 0};
  cd_values[0] = H5Z_SPERR_pack_meta(rank, is_float, mode);
  memcpy(&cd_values[1], &user_cd_values[1], sizeof(double));
  cd_values[3] = chunks[0];
  cd_values[4] = chunks[1];
  if (rank == 2)
    H5Pmodify_filter(dcpl_id, H5Z_FILTER_SPERR, H5Z_FLAG_MANDATORY, 5, cd_values);
  else {
    cd_values[5] = chunks[2];
    H5Pmodify_filter(dcpl_id, H5Z_FILTER_SPERR, H5Z_FLAG_MANDATORY, 6, cd_values);
  }

  return 0;
}
static size_t H5Z_filter_sperr(unsigned int flags,
                               size_t cd_nelmts,
                               const unsigned int cd_values[],
                               size_t nbytes,
                               size_t* buf_size,
                               void** buf)
{
  /* Extract info from cd_values[] */
  int rank, is_float, mode;
  H5Z_SPERR_unpack_meta(cd_values[0], &rank, &is_float, &mode);
  double quality;
  memcpy(&quality, &cd_values[1], sizeof(quality));
  unsigned int dims[3] = {1, 1, 1};
  dims[0] = cd_values[3];
  dims[1] = cd_values[4];
  if (rank == 3)
    dims[2] = cd_values[5];

  /* Decompression */
  if (flags & H5Z_FLAG_REVERSE) {
    void* dst = NULL; /* buffer to hold the decompressed data */
    int ret = 0;
    if (rank == 2)
      ret = sperr_decomp_2d(*buf, nbytes, is_float, dims[0], dims[1], &dst);
    else {
      size_t dimx, dimy, dimz;
      ret = sperr_decomp_3d(*buf, nbytes, is_float, 1, &dimx, &dimy, &dimz, &dst);
    }
    if (ret != 0) {
      if (dst)
        free(dst);
      H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
              "SPERR decompression failed.");
      return 0;
    }

    size_t dst_len = (is_float ? 4ul : 8ul) * dims[0] * dims[1] * dims[2];
    if (dst_len < *buf_size) { /* Re-use the input buffer */
      memcpy(*buf, dst, dst_len);
      free(dst);
    }
    else { /* Point to the new buffer */
      free(*buf);
      *buf = dst;
      *buf_size = dst_len;
    }

    return dst_len;

  }      /* Finish Decompression */
  else { /* Compression */

    /* Sanity check on the data size. */
    if ((is_float ? 4ul : 8ul) * dims[0] * dims[1] * dims[2] != nbytes) {
      H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADSIZE,
              "Compression: input buffer len isn't right.");
      return 0;
    }

    void* dst = NULL; /* buffer to hold compressed bitstream */
    size_t dst_len = 0;
    int ret = 0;
    ;

    if (rank == 2)
      ret = sperr_comp_2d(*buf, is_float, dims[0], dims[1], mode, quality, 0, &dst, &dst_len);
    else
      ret = sperr_comp_3d(*buf, is_float, dims[0], dims[1], dims[2], dims[0], dims[1], dims[2],
                          mode, quality, 1, &dst, &dst_len);
    if (ret != 0) {
      if (dst)
        free(dst);
      H5Epush(H5E_DEFAULT, __FILE__, __func__, __LINE__, H5E_ERR_CLS, H5E_PLINE, H5E_BADVALUE,
              "SPERR compression failed.");
      return 0;
    }

    if (dst_len < *buf_size) { /* Re-use the input buffer */
      memcpy(*buf, dst, dst_len);
      free(dst);
    }
    else { /* Point to the new buffer */
      free(*buf);
      *buf = dst;
      *buf_size = dst_len;
    }

    return dst_len;

  } /* Finish compression */
}

const H5Z_class2_t H5Z_SPERR_class_t = {H5Z_CLASS_T_VERS, /* H5Z_class_t version */
                                        H5Z_FILTER_SPERR, /* Filter id number    */
                                        1,                /* encoder_present flag (set to true) */
                                        1,                /* decoder_present flag (set to true) */
                                        "H5Z-SPERR",      /* Filter name for debugging  */
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
