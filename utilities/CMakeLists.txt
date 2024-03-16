add_executable( example-3d example-3d.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( example-3d PUBLIC ${HDF5_LIBRARIES} )

add_executable( example-2d example-2d.c )
include_directories(${HDF5_INCLUDE_DIR})
target_link_libraries( example-2d PUBLIC ${HDF5_LIBRARIES} )
