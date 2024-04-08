# H5Z-SPERR
This is an HDF5 Plugin for the [SPERR](https://github.com/ncar/sperr) compressor.
It is registered with the HDF Group with a plugin ID of `32028`.

## Build and Install
Needless to say, `H5Z-SPERR` depends on both `HDF5` and `SPERR`. 
After HDF5 and SPERR are installed, `H5Z-SPERR` can be configured and built using `cmake`:
```bash
export HDF5_ROOT=/path/to/your/preferred/HDF5/installation
cmake -DSPERR_INSTALL_DIR=/path/to/SPERR/installation     \
      -DCMAKE_INSTALL_PREFIX=/path/to/install/this/plugin \
      /path/to/H5Z-SPERR/source/code
make install
```
The plugin library file `libh5z-sperr.so` will be placed at directory `/path/to/install/this/plugin`.

## Use As a Dynamically Loaded Plugin
Using the [dynamically loaded plugin](https://docs.hdfgroup.org/hdf5/rfc/HDF5DynamicallyLoadedFilters.pdf) mechanism by HDF5,
one may use `H5Z-SPERR` by simply setting the environment variable `HDF5_PLUGIN_PATH` to the directory containing the plugin
library file:
```bash
export HDF5_PLUGIN_PATH=/path/to/install/this/plugin
```
The user program does not need to link to this plugin or SPERR; it only needs to specify the plugin ID of `32028`.

##  Specify `cd_values[]` in The Programming Interface
To apply SPERR compression using the HDF5 programming interface, one needs to specify 1) what compression mode and 2)
what compression quality to use. Supported compression modes and qualities are summarized below:
| Mode No.      | Mode Meaning         | Meaningful Quality Range  |
| ------------- | -------------------- | ------------------------- |
| 1             | Fixed bit-per-pixel (BPP) | 0.0 < quality < 64.0 |
| 2             | Fixed peak signal-to-noise ratio (PSNR) | 0.0 < quality |
| 3             | Fixed point-wise error (PWE)            | 0.0 < quality |

`H5Z-SPERR` organizes the compression mode and quality information in a 32-bit `unsigned int`,
which can be saved in `cd_values[]` that is the input to HDF5 routines such as `H5Pset_filter()`.
Users need to include the header [`h5z-sperr.h`](https://github.com/NCAR/H5Z-SPERR/blob/main/include/h5z-sperr.h) from this repository
and call the `unsigned int H5Z_SPERR_make_cd_values(int mode, double quality){}` function 
to have these two pieces of information correctly encoded. For example:
```C++
int mode = 3;              /* Fixed PWE compression */
double quality = 1e-6;     /* PWE tolerance = 1e-6 */
unsigned int cd_values = H5Z_SPERR_make_cd_values(mode, quality);   /* Generate cd_values */
H5Pset_filter(prop, 32028, H5Z_FLAG_MANDATORY, 1, &cd_values);      /* Specify SPERR compression in HDF5 */
```
