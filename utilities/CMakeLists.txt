add_executable( md5-example md5-example.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( md5-example PUBLIC ${HDF5_LIBRARIES} )

add_executable( sperr-example-3d sperr-example-3d.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( sperr-example-3d PUBLIC ${HDF5_LIBRARIES} )

add_executable( sperr-example-2d sperr-example-2d.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( sperr-example-2d PUBLIC ${HDF5_LIBRARIES} )
