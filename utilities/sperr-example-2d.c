#include <stdlib.h>
#include <string.h>
#include <math.h>
 
#include "hdf5.h"

#define H5Z_FILTER_SPERR 34000

int main (int argc, char* argv[])
{
    if (argc != 4) {
      printf("Usage: sperr-example-2d 2D_double_file dimx dimy \n");
      exit (1);
    }
    const char* filename = argv[1];
    hsize_t     dims[2] = {atoi(argv[2]), atoi(argv[3])};  /* dataset dimensions */
    hsize_t     chunks[2] = {dims[0], dims[1]};            /* dataset chunks */
    hid_t       file, dataset, datatype, dataspace;        /* handles */
    herr_t      status;                             

    /* 
     * Data  and output buffer initialization. 
     */
    size_t      total_vals = dims[0] * dims[1];
    double*     data = (double*)malloc(total_vals * sizeof(double));
    FILE*       fp = fopen(filename, "rb");
    if (!fp) {
      printf("File open error: %s\n", filename);
      exit (1);
    }
    size_t nread = fread(data, sizeof(double), total_vals, fp);
    fclose(fp);
    fp = NULL;
    if (nread != total_vals) {
      printf("File read error: %s\n", filename);
      exit (1);
    }

    /*
     * Create a new file using H5F_ACC_TRUNC access, default file creation
     * properties, and default file access properties.
     */
    const char* h5name = "sperr-example.h5";
    file = H5Fcreate(h5name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed size dataset. 
     */
    dataspace = H5Screate_simple(2, dims, NULL); 

    /*
     * Setup SPERR filter using dataset creation properties
     */
    hid_t prop = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(prop, 2, chunks);
    htri_t filter_avail = H5Zfilter_avail(H5Z_FILTER_SPERR);
    if (filter_avail > 0)
      printf("Filter %d available!\n", H5Z_FILTER_SPERR);
    else
      printf("Filter %d unavailable!\n", H5Z_FILTER_SPERR);
    /*    
     *    mode == 1 --> fixed bit-per-pixel (BPP)
     *    mode == 2 --> fixed peak signal-to-noise ratio (PSNR)
     *    mode == 3 --> fixed point-wise error (PWE)
     */
    unsigned int cd_values[3] = {3, 0, 0};
    double quality = 1e-6;
    memcpy(&cd_values[1], &quality, sizeof(quality));
    status = H5Pset_filter(prop, H5Z_FILTER_SPERR, H5Z_FLAG_MANDATORY, 3, cd_values);

    /*
     * Create a dataset WITH compression, and write to it!  
     */
    const char* dsname = "f64_sperr";
    dataset = H5Dcreate(file, dsname, H5T_NATIVE_DOUBLE, dataspace, H5P_DEFAULT, prop, H5P_DEFAULT);
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Clean up writing.
     */
    H5Dclose(dataset);
    H5Pclose(prop);
    H5Sclose(dataspace);
    H5Fclose(file);

    /*
     * Open the file
     */
    file = H5Fopen(h5name, H5F_ACC_RDONLY, H5P_DEFAULT);
    dataset = H5Dopen(file, dsname, H5P_DEFAULT);
    dataspace = H5Dget_space(dataset);
    hssize_t npoints = H5Sget_simple_extent_npoints(dataspace);

    /*
     * Read the compressed array
     */
    dataset = H5Dopen(file, dsname, H5P_DEFAULT);
    double* comp = (double*)malloc(npoints * sizeof(double));
    H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, comp);

    /* Find the maximum difference */
    double max = 0.0;
    for (size_t i = 0; i < npoints; i++)
      if (fabsf(comp[i] - data[i]) > max)
        max = fabsf(comp[i] - data[i]);

    printf("max diff = %g\n", max);

    /* clean up reading */
    H5Dclose(dataset);
    H5Sclose(dataspace);
    H5Fclose(file);
    free(data);
    free(comp);

    return 0;
}
