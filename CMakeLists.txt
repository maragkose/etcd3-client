cmake_minimum_required(VERSION 3.5)

project(etcd3cppapi CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include (ExternalProject)
include(FindProtobuf)
include(FindGRPC++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -ggdb -std=c++14")

ExternalProject_Add(
etcd_repo
GIT_REPOSITORY "https://github.com/coreos/etcd.git"
GIT_TAG "fc00305a2e59b4c2d4a53c9fbb4d30741a96ea67"
UPDATE_COMMAND ""
PATCH_COMMAND ""
CONFIGURE_COMMAND ""
SOURCE_DIR "${CMAKE_SOURCE_DIR}/etcd_repo"
INSTALL_COMMAND ""
BUILD_COMMAND ""
)

ExternalProject_Add(
protobuf_repo
GIT_REPOSITORY "https://github.com/gogo/protobuf"
GIT_TAG "2752d97bbd91927dd1c43296dbf8700e50e2708c" 
UPDATE_COMMAND ""
PATCH_COMMAND ""
CONFIGURE_COMMAND ""
SOURCE_DIR "${CMAKE_SOURCE_DIR}/protobuf_repo"
INSTALL_COMMAND ""
BUILD_COMMAND ""
)

ExternalProject_Add(
googleapis_repo
GIT_REPOSITORY "https://github.com/googleapis/googleapis"
GIT_TAG "72ebf10ad5b2355df038e275a1543418ea8ac109"
UPDATE_COMMAND ""
PATCH_COMMAND ""
CONFIGURE_COMMAND ""
SOURCE_DIR "${CMAKE_SOURCE_DIR}/googleapis_repo"
INSTALL_COMMAND ""
BUILD_COMMAND ""
)

# Protobuf
find_package(Protobuf 3.0 REQUIRED)
find_package(GRPC++ REQUIRED)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/etcd/auth/authpb")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/etcd/mvcc/mvccpb")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gogoproto")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/google/api")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/google/protobuf")

set(SRC_PROTO_FILES 
    "protobuf_repo/gogoproto/gogo.proto"
    "googleapis_repo/google/api/http.proto"
    "protobuf_repo/protobuf/google/protobuf/descriptor.proto"
    "googleapis_repo/google/api/annotations.proto"
    "etcd_repo/auth/authpb/auth.proto"
    "etcd_repo/mvcc/mvccpb/kv.proto"
    "etcd_repo/etcdserver/etcdserverpb/rpc.proto"
)
set(DST_PROTO_FILES 
    "gogoproto/gogo.proto"
    "google/api/http.proto"
    "google/protobuf/descriptor.proto"
    "google/api/annotations.proto"
    "etcd/auth/authpb/auth.proto"
    "etcd/mvcc/mvccpb/kv.proto"
    "rpc.proto"
)

list(LENGTH SRC_PROTO_FILES LEN)
math(EXPR PROTO_LIST_NUM "${LEN} - 1")

foreach (LIST_ITEM RANGE ${PROTO_LIST_NUM})
    list(GET SRC_PROTO_FILES ${LIST_ITEM} SRC_PROTO_FILE) 
    list(GET DST_PROTO_FILES ${LIST_ITEM} DST_PROTO_FILE) 
    get_filename_component(DST_PROTO_FILE_DIR ${DST_PROTO_FILE} DIRECTORY)
    
    add_custom_command (OUTPUT "${DST_PROTO_FILE}"
        COMMENT "Copying ${SRC_PROTO_FILE} to ${DST_PROTO_FILE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_PROTO_FILE}" "./${DST_PROTO_FILE_DIR}/."
        DEPENDS etcd_repo googleapis_repo protobuf_repo
    )
endforeach() 

foreach(PROTO_FILE ${DST_PROTO_FILES})
    set(CC_EXT ".pb.cc")
    set(H_EXT ".pb.h")
    get_filename_component(FILE_WE ${PROTO_FILE} NAME_WE)
    get_filename_component(FILE_LOC ${PROTO_FILE} DIRECTORY)
    set(CC_FILE "${CMAKE_CURRENT_BINARY_DIR}/${FILE_LOC}/${FILE_WE}${CC_EXT}")
    set(H_FILE "${CMAKE_CURRENT_BINARY_DIR}/${FILE_LOC}/${FILE_WE}${H_EXT}")
    
    add_custom_command (OUTPUT ${CC_FILE} ${H_FILE}
        COMMENT "Generating ${CC_FILE} and ${H_FILE} from ${PROTO_FILE}..."
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` ${PROTO_FILE}
        DEPENDS "${PROTO_FILE}" 
    )
    
    set(CC_FILES ${CC_FILES} ${CC_FILE})
    set(H_FILES ${H_FILES} ${H_FILE})
endforeach()

add_custom_command (OUTPUT rpc.grpc.pb.cc rpc.grpc.pb.h
    COMMENT "Generating  rpc.grpc.pb.cc and rpc.grpc.pb.h..."
    COMMAND  ${PROTOBUF_PROTOC_EXECUTABLE} -I. --grpc_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` rpc.proto    
    DEPENDS rpc.proto 
)

include_directories(${CMAKE_CURRENT_BINARY_DIR} 
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_library(etcd3cppapi SHARED ${CC_FILES} 
                               ${H_FILES} 
                               rpc.grpc.pb.cc 
                               rpc.grpc.pb.h  
                               src/Client.cpp)

target_link_libraries(etcd3cppapi ${GRPC++_LIBRARY} ${PROTOBUF_LIBRARY})

add_executable(etcd3client  
                           rpc.grpc.pb.cc 
                           rpc.grpc.pb.h  
                           src/Client.cpp
                           tst/etcd3client.cpp)
add_dependencies(etcd3client etcd3cppapi)
target_link_libraries(etcd3client etcd3cppapi)



