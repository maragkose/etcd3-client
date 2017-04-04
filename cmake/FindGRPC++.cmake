
find_path(GRPC_INCLUDE_DIR
    NAMES grpc++.hpp
    PATHS /usr/local/ 
    DOC "The grpc++ include directory"
)

find_library(GRPCXX_LIBRARY
NAMES grpc++
PATHS /usr/local/lib
DOC "Grpc++ library"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GRPC DEFAULT_MSG GRPC_INCLUDE_DIR GRPCXX_LIBRARY)
if (GRPC_FOUND)
    message(STATUS "Found grpc++ library ${GRPCXX_LIBRARY}")
    set(GRPC++_LIBRARY ${GRPCXX_LIBRARY} )
    set(GRPC++_INCLUDE_DIR ${GRPC_INCLUDE_DIR} )
    set(GRPC_DEFINITIONS )
endif()

mark_as_advanced(GRPCXX_INCLUDE_DIR GRPCXX_LIBRARY)

