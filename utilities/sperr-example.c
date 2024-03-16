/*  
 *  This example writes data to the HDF5 file.
 *  Data conversion is performed during write operation.  
 *  Example 1: https://web.mit.edu/fwtools_v3.1.0/www/H5.intro.html
 */

#include <stdlib.h>
 
#include "hdf5.h"

#define NX     20
#define NY     20
#define NZ     20

#define H5Z_FILTER_SPERR 34000

int main (void)
{
    hid_t       file, dataset;              /* file and dataset handles */
    hid_t       datatype, dataspace;        /* handles */
    hsize_t     dims[3] = {NZ, NZ, NZ};     /* dataset dimensions */
    hsize_t     chunks[3] = {10, 10, 10};   /* dataset chunks */
    herr_t      status;                             

    /* 
     * Data  and output buffer initialization. 
     */
    size_t      total_vals = NX * NY * NZ;
    float*      data = (float*)malloc(total_vals * sizeof(float));          /* data to write */
    for (size_t i = 0; i < total_vals; i++)
	    data[i] = 0.1f * i;

    /*
     * Create a new file using H5F_ACC_TRUNC access, default file creation
     * properties, and default file access properties.
     */
    file = H5Fcreate("sample2.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed size dataset. 
     */
    dataspace = H5Screate_simple(3, dims, NULL); 

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate(file, "f32_array", H5T_NATIVE_FLOAT, dataspace, 
                        H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Setup SPERR filter
     * Setup dataset creation properties
     */
    hid_t prop = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(prop, 3, chunks);
    htri_t filter_avail = H5Zfilter_avail(H5Z_FILTER_SPERR);
    if (filter_avail > 0)
      printf("Filter %d available!\n", H5Z_FILTER_SPERR);
    else
      printf("Filter %d unavailable!\n", H5Z_FILTER_SPERR);
    unsigned int cd_values[3] = {1, 2, 3};
    status = H5Pset_filter(prop, H5Z_FILTER_SPERR, H5Z_FLAG_MANDATORY, 3, cd_values);

    /*
     * Create a dataset WITH compression, and write to it!  
     */
    hid_t dataset2 = H5Dcreate(file, "f32_sperr", H5T_NATIVE_FLOAT, dataspace, 
                               H5P_DEFAULT, prop, H5P_DEFAULT);
    status = H5Dwrite(dataset2, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Close/release resources.
     */
    H5Dclose(dataset2);
    H5Pclose(prop);
    H5Sclose(dataspace);
    H5Dclose(dataset);
    H5Fclose(file);
    free (data);

    /*
     * Open the file
     */
    file = H5Fopen("sample2.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
    dataset = H5Dopen(file, "f32_array", H5P_DEFAULT);
    dataspace = H5Dget_space(dataset);
    hssize_t npoints = H5Sget_simple_extent_npoints(dataspace);
    H5Sclose(dataspace);

    /*
     * Read the original array
     */
    float* orig = (float*)malloc(npoints * sizeof(float));
    H5Dread(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, orig);
    H5Dclose(dataset);

    /*
     * Read the compressed array
     */
    dataset2 = H5Dopen(file, "f32_sperr", H5P_DEFAULT);
    float* comp = (float*)malloc(npoints * sizeof(float));
    H5Dread(dataset2, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, comp);
    H5Dclose(dataset2);

    /* claim that orig == comp */
    int fail = 0;
    for (size_t i = 0; i < npoints; i++)
      if (orig[i] != comp[i]) {
        printf("orig = %f, comp = %f\n", orig[i], comp[i]);
        fail = 1;
        break;
      }
    if (fail)
      printf("orig does NOT equal comp!\n");
    else
      printf("orig equals comp!\n");

    /* clean up */
    H5Fclose(file);
    free(orig);
    free(comp);

    return 0;
}
