
find_path(GRPC_INCLUDE_DIR
    NAMES grpc/grpc.h
    DOC "The grpc include directory"
)

find_path(GRPCXX_INCLUDE_DIR
    NAMES grpc++/grpc++.h
    HINTS ${GRPC_INCLUDE_DIR}
    DOC "The grpc++ include directory"
)

find_library(GRPC_LIBRARY
    NAMES libgrpc grpc
)

find_library(GRPCXX_LIBRARY
    NAMES libgrpc++ grpc++
    HINTS ${GRPC_LIBRARY}
    DOC "Grpc++ library"
)
find_program(GRPCXX_PROTOC_PLUGIN
             NAMES grpc_cpp_plugin
             HINTS ${GRPC_LIB_DIR}/../bin)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Grpc++ DEFAULT_MSG 
                                  GRPC_INCLUDE_DIR 
                                  GRPCXX_LIBRARY
                                  GRPC_LIBRARY
                                  GRPCXX_INCLUDE_DIR
)
if (GRPC++_FOUND)
    message(STATUS "Found grpc++ library ${GRPCXX_LIBRARY}")
    set(GRPC++_LIBRARY ${GRPCXX_LIBRARY} )
    set(GRPC++_INCLUDE_DIR ${GRPC_INCLUDE_DIR} )
    set(GRPC_DEFINITIONS )
endif()

mark_as_advanced(GRPCXX_INCLUDE_DIR GRPCXX_LIBRARY GRPC_INCLUDE_DIR GRPC_LIBRARY )

