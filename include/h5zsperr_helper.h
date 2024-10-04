/*
 * This file contains a few helper functions for the H5Z-SPERR filter.
 */

#ifndef H5ZSPERR_HELPER_H
#define H5ZSPERR_HELPER_H

/*
 * Pack additional information about the input data into an `unsigned int`.
 * It returns the encoded unsigned int, which shouldn't be zero.
 * The packing function is called by `set_local()` to prepare information
 * for `H5Z_filter_sperr()`, which calls the unpack function.
 */
unsigned int h5zsperr_pack_data_type(int rank, int dtype);
void h5zsperr_unpack_data_type(unsigned int meta, int* rank, int* dtype);

#endif
