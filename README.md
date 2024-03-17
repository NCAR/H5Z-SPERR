# H5Z-SPERR
This is an HDF5 Plugin for the [SPERR](https://github.com/ncar/sperr) Compressor.
At this moment, it's not registered with the HDF group, and uses an experimental
HDF plugin ID of [`34000`](https://github.com/NCAR/H5Z-SPERR/blob/7f26424fb225e6f306d750c6c0f00e85c66b2d02/src/h5z-sperr.c#L11).

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
The user program does not need to link to this plugin or SPERR; it only needs to specify the plugin ID of `34000`.

##  Specify `cd_values[]` in The Programming Interface
To apply SPERR compression using the HDF5 programming interface, one needs to specify 1) what compression mode and 2)
what compression quality to use. Supported compression modes and qualities are summarized below:
| Mode No.      | Mode Meaning         | Meaningful Quality Range  |
| ------------- | -------------------- | ------------------------- |
| 1             | Fixed bit-per-pixel (BPP) | 0.0 < quality < 64.0 |
| 2             | Fixed peak signal-to-noise ratio (PSNR) | 0.0 < quality |
| 3             | Fixed point-wise error (PWE)            | 0.0 < quality |

`H5Z-SPERR` organizes the compression mode and quality information in an `unsigned int cd_values[3]` object, which
is passed to the HDF5 library. Specifically, `cd_values[0]` keeps the compression mode, and `cd_values[1-2]` keeps
the compression quality info as a 64-bit `double`. A few examples to specify `cd_values[]` are:
```C++
unsigned int cd_values[3];  /* will be passed to the HDF5 library */
double quality;

/* Use fixed-BPP compression mode: 4 bit-per-pixel. */
cd_values[0] = 1;
quality = 4.0;
memcpy(&cd_values[1], &quality, sizeof(quality));

/* Use fixed-PSNR compression mode: 150 decibel. */
cd_values[0] = 2;
quality = 150.0;
memcpy(&cd_values[1], &quality, sizeof(quality));

/* Use fixed-PWE compression mode: tolerance = 1e-6. */
cd_values[0] = 3;
quality = 1e-6;
memcpy(&cd_values[1], &quality, sizeof(quality));
```
