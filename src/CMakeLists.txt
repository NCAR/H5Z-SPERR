add_library( h5z-md5 h5z-md5.c )
target_link_libraries( h5z-md5 PUBLIC ${HDF5_LIBRARIES} )
target_include_directories( h5z-md5 PUBLIC ${HDF5_INCLUDE_DIR})
