/*
 * This file contains a few helper functions for the H5Z-SPERR filter.
 */

#ifndef H5ZSPERR_HELPER_H
#define H5ZSPERR_HELPER_H

#include <stdlib.h>

#define LARGE_MAGNITUDE 1e35

/*
 * Pack and unpack additional information about the input data into an integer.
 * It returns the encoded unsigned int, which shouldn't be zero.
 * The packing function is called by `set_local()` to prepare information
 * for `H5Z_filter_sperr()`, which calls the unpack function.
 */
unsigned int h5zsperr_pack_data_type(int rank, int is_float, int missing_val_mode);
void h5zsperr_unpack_data_type(unsigned int meta, int* rank, int* is_float, int* missing_val_mode);

/*
 * Check if an input array really has missing values.
 */
int h5zsperr_has_nan(const void* buf, size_t nelem, int is_float);
int h5zsperr_has_large_mag(const void* buf, size_t nelem, int is_float);
int h5zsperr_has_specific_f32(const void* buf, size_t nelem, float val);
int h5zsperr_has_specific_f64(const void* buf, size_t nelem, double val);

#endif
