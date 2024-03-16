add_library( h5z-sperr h5z-sperr.c )
target_include_directories( h5z-sperr PUBLIC ${HDF5_INCLUDE_DIR} PUBLIC ${SPERR_INCLUDE_DIRS} )
target_link_libraries( h5z-sperr PUBLIC ${HDF5_LIBRARIES} PUBLIC PkgConfig::SPERR )
