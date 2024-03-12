add_executable( md5-example md5-example.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( md5-example PUBLIC ${HDF5_LIBRARIES} )

