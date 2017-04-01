#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include "rpc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using etcdserverpb::KV;
using etcdserverpb::PutRequest;
using etcdserverpb::PutResponse;
using etcdserverpb::RequestOp;
using etcdserverpb::RangeRequest;

void cpp_put_key(){
    std::shared_ptr<Channel> channel = grpc::CreateChannel("127.0.0.1:2379", grpc::InsecureChannelCredentials());

    std::unique_ptr<KV::Stub> stub_;
    stub_ = KV::NewStub(channel);
   
    ClientContext context;
    PutResponse put_response;
 
    std::unique_ptr<PutRequest> put_request(new PutRequest());
    put_request->set_key("Python");   
    put_request->set_value("Value");   
 
    Status status = stub_->Put(&context, *(put_request.get()), &put_response);    

/*
    if (status.ok()) {
       return  "Write ok";
    } else {
      //std::cout << status.error_code() << ": " << status.error_message()
      //         << std::endl;
      return   "RPC failed";
    }
*/


}

#include <boost/python.hpp>

BOOST_PYTHON_MODULE(cpp_put)
{
    using namespace boost::python;
    def("cpp_put_key", cpp_put_key);
}
