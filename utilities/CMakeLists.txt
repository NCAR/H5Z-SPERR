add_executable( md5-example md5-example.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( md5-example PUBLIC ${HDF5_LIBRARIES} )

add_executable( sperr-example sperr-example.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( sperr-example PUBLIC ${HDF5_LIBRARIES} )
