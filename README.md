# etcd3-client
Thin cpp wrapper for etc v3 cpp api

==Currently under development.==

Generated files are currently hard commited in the repo. 
Compile:
g++ -I./ -std=c++11 -o client Client.cpp etcdserver.grpc.pb.cc etcdserver.pb.cc kv.pb.cc rpc.grpc.pb.cc rpc.pb.cc auth.pb.cc auth.grpc.pb.cc -lprotobuf -lgrpc++
