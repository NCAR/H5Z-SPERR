/*  
 *  This example writes data to the HDF5 file.
 *  Data conversion is performed during write operation.  
 *  Example 1: https://web.mit.edu/fwtools_v3.1.0/www/H5.intro.html
 */
 
#include "hdf5.h"

#define NX     5
#define NY     6
#define RANK   2

#define H5Z_FILTER_MD5 33000

int main (void)
{
    hid_t       file, dataset;         /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dims[2];               /* dataset dimensions */
    herr_t      status;                             
    int         data[NX][NY];          /* data to write */
    int         i, j;

    /* 
     * Data  and output buffer initialization. 
     */
    for (j = 0; j < NX; j++) {
	    for (i = 0; i < NY; i++)
	      data[j][i] = i + j;
    }     


    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate("sample.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset. 
     */
    dims[0] = NX;
    dims[1] = NY;
    dataspace = H5Screate_simple(RANK, dims, NULL); 

    /* 
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_INT);
    status = H5Tset_order(datatype, H5T_ORDER_LE);

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate(file, "int_array", datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* query the data type */
    hid_t datatype2  = H5Dget_type(dataset);     /* datatype identifier */ 
    hid_t class2     = H5Tget_class(datatype2);
    hid_t order2     = H5Tget_order(datatype2);
    int   size2      = H5Tget_size(datatype);
    if (class2 == H5T_INTEGER) 
      printf("Data set has INTEGER type \n");
    if (order2 == H5T_ORDER_LE) 
      printf("Little endian order \n");
    printf("Data size is %d \n", size2);

    /* query the data space */
    hid_t dataspace2 = H5Dget_space(dataset);  
    int   rank2      = H5Sget_simple_extent_ndims(dataspace2);
    hsize_t dims2[2], maxdims[2];
    status = H5Sget_simple_extent_dims(dataspace2, dims2, maxdims);
    printf("rank %d, dimensions %llu x %llu, max dims %llu x %llu \n", rank2, dims2[0], dims2[1],
            maxdims[0], maxdims[1]);

    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Setup MD5 filter
     */
    /* Setup dataset creation properties */
    hid_t prop = H5Pcreate(H5P_DATASET_CREATE);
    H5Pset_chunk(prop, RANK, dims);
    htri_t filter_avail = H5Zfilter_avail(H5Z_FILTER_MD5);
    if (filter_avail > 0)
      printf("Filter %d available!\n", H5Z_FILTER_MD5);
    else
      printf("Filter %d unavailable!\n", H5Z_FILTER_MD5);
    status = H5Pset_filter(prop, H5Z_FILTER_MD5, H5Z_FLAG_MANDATORY, 0, NULL);

    /*
     * Create a dataset WITH compression
     */
    hid_t dataset2 = H5Dcreate(file, "int_md5", datatype, dataspace, H5P_DEFAULT, prop, H5P_DEFAULT);
    status = H5Dwrite(dataset2, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Close/release resources.
     */
    H5Dclose(dataset2);
    H5Pclose(prop);
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);

    return 0;
}
