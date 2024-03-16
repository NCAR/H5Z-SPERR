add_library( h5z-md5 h5z-md5.c )
target_link_libraries( h5z-md5 PUBLIC ${HDF5_LIBRARIES} )
target_include_directories( h5z-md5 PUBLIC ${HDF5_INCLUDE_DIR} )

add_library( h5z-sperr h5z-sperr.c )
target_include_directories( h5z-sperr PUBLIC ${HDF5_INCLUDE_DIR} PUBLIC ${SPERR_INCLUDE_DIRS} )
target_link_libraries( h5z-sperr PUBLIC ${HDF5_LIBRARIES} PUBLIC PkgConfig::SPERR )
