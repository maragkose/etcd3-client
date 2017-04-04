cmake_minimum_required(VERSION 3.5)

include (ExternalProject)
include(FindProtobuf)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

ExternalProject_Add(
etcd
GIT_REPOSITORY "https://github.com/coreos/etcd.git"
GIT_TAG "fc00305a2e59b4c2d4a53c9fbb4d30741a96ea67"
UPDATE_COMMAND ""
PATCH_COMMAND ""
CONFIGURE_COMMAND ""
SOURCE_DIR "${CMAKE_SOURCE_DIR}/etcd"
INSTALL_COMMAND ""
BUILD_COMMAND ""
)

ExternalProject_Add(
protobuf
GIT_REPOSITORY "https://github.com/gogo/protobuf"
GIT_TAG "2752d97bbd91927dd1c43296dbf8700e50e2708c" 
UPDATE_COMMAND ""
PATCH_COMMAND ""
CONFIGURE_COMMAND ""
SOURCE_DIR "${CMAKE_SOURCE_DIR}/protobuf"
INSTALL_COMMAND ""
BUILD_COMMAND ""
)

ExternalProject_Add(
googleapis
GIT_REPOSITORY "https://github.com/googleapis/googleapis"
GIT_TAG "72ebf10ad5b2355df038e275a1543418ea8ac109"
UPDATE_COMMAND ""
PATCH_COMMAND ""
CONFIGURE_COMMAND ""
SOURCE_DIR "${CMAKE_SOURCE_DIR}/googleapis"
INSTALL_COMMAND ""
BUILD_COMMAND ""
)

# Protobuf
find_package(Protobuf)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/etcd/auth/authpb")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/etcd/mvcc/mvccpb")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/gogoproto")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/google/api")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/google/protobuf")


add_custom_target (copyprotos
COMMENT "Copying proto files.... "
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/etcd/etcdserver/etcdserverpb/rpc.proto        ${CMAKE_CURRENT_BINARY_DIR}/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/etcd/auth/authpb/auth.proto                   ${CMAKE_CURRENT_BINARY_DIR}/etcd/auth/authpb/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/etcd/mvcc/mvccpb/kv.proto                     ${CMAKE_CURRENT_BINARY_DIR}/etcd/mvcc/mvccpb/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/googleapis/google/api/annotations.proto       ${CMAKE_CURRENT_BINARY_DIR}/google/api/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/googleapis/google/api/http.proto              ${CMAKE_CURRENT_BINARY_DIR}/google/api/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/protobuf/google/protobuf/descriptor.proto ${CMAKE_CURRENT_BINARY_DIR}/google/protobuf/
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/protobuf/gogoproto/gogo.proto                 ${CMAKE_CURRENT_BINARY_DIR}/gogoproto/
DEPENDS etcd googleapis protobuf
)

add_custom_target (generatecode 
  COMMENT "Generating source code from proto files..." 
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` rpc.proto 
  COMMAND  protoc -I. --grpc_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` rpc.proto 
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` etcd/auth/authpb/auth.proto 
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` etcd/mvcc/mvccpb/kv.proto 
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` google/api/annotations.proto 
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` google/api/http.proto
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` google/protobuf/descriptor.proto
  COMMAND  protoc -I. --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --plugin=`which grpc_cpp_plugin` gogoproto/gogo.proto
  DEPENDS copyprotos
)

set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/rpc.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/rpc.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/rpc.grpc.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/rpc.grpc.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/etcd/auth/authpb/auth.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/etcd/auth/authpb/auth.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/etcd/mvcc/mvccpb/kv.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/etcd/mvcc/mvccpb/kv.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/google/api/annotations.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/google/api/annotations.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/google/api/http.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/google/api/http.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/google/protobuf/descriptor.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/google/protobuf/descriptor.pb.h")
set (CC_FILES ${CC_FILES} "${CMAKE_CURRENT_BINARY_DIR}/gogoproto/gogo.pb.cc")
set (H_FILES ${H_FILES}   "${CMAKE_CURRENT_BINARY_DIR}/gogoproto/gogo.pb.h")

set_source_files_properties(${CC_FILES} PROPERTIES GENERATED TRUE)
set_source_files_properties(${H_FILES} PROPERTIES GENERATED TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(client ${CC_FILES} ${H_FILES} src/Client.cpp)
target_link_libraries(client grpc++ ${PROTOBUF_LIBRARY})

add_dependencies(client generatecode)



