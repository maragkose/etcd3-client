#ifndef ETCDV3_CLIENT
#define ETCDV3_CLIENT

#include "rpc.grpc.pb.h"

#include <grpc++/grpc++.h>
#include <map>

using grpc::Channel;
using grpc::ClientContext;
using etcdserverpb::KV;
using etcdserverpb::Watch;
using etcdserverpb::Lease;
using etcdserverpb::PutRequest;
using etcdserverpb::PutResponse;
using etcdserverpb::RangeRequest;
using etcdserverpb::RangeResponse;
using grpc::Status;

//--------------------------------------------------
class Client {
//--------------------------------------------------

public:

    explicit Client(std::shared_ptr<Channel> channel)
    : m_kvStub(KV::NewStub(channel)),
      m_watchStub(Watch::NewStub(channel)),
      m_leaseStub(Lease::NewStub(channel)) 
    {
    }

    explicit Client(std::string address) 
    : Client(grpc::CreateChannel(address, grpc::InsecureChannelCredentials())) 
    {
    }
    
    Status put(const std::string &key, const std::string &value){
        
        ClientContext context;
        PutResponse put_response;
 
        std::unique_ptr<PutRequest> put_request(new PutRequest());
        put_request->set_key(key);   
        put_request->set_value(value);   
        put_request->set_prev_kv(false);
        Status putStatus = m_kvStub->Put(&context, *(put_request.get()), &put_response); 
        return putStatus; 
    }
    
    Status put(const std::string &key, const std::string &value, std::pair<std::string, std::string> &prevKV) {
        
        ClientContext context;
        PutResponse put_response;
 
        std::unique_ptr<PutRequest> put_request(new PutRequest());
        put_request->set_key(key);   
        put_request->set_value(value);   
        put_request->set_prev_kv(true);
        Status putStatus = m_kvStub->Put(&context, *(put_request.get()), &put_response);
    
        prevKV.first = put_response.prev_kv().key();
        prevKV.first = put_response.prev_kv().value();
        return putStatus; 
    }

    Status put(const std::string &key, const std::string &value, int64_t lease){
        
        ClientContext context;
        PutResponse put_response;
 
        std::unique_ptr<PutRequest> put_request(new PutRequest());
        put_request->set_key(key);   
        put_request->set_value(value);   
        put_request->set_prev_kv(false);
        put_request->set_lease(lease);
        Status putStatus = m_kvStub->Put(&context, *(put_request.get()), &put_response); 
        return putStatus; 
    }
    
    const std::string get(const std::string key){
        
        ClientContext context;
        RangeRequest getRequest;
        RangeResponse getResponse;
        
        getRequest.set_key(key);
    
        Status getStatus = m_kvStub->Range(&context, getRequest , &getResponse);
        
        return getResponse.kvs(0).value();     
    }

    Status getFromKey(const std::string key, std::map<std::string, std::string> & pairs) {
        
        ClientContext context;
        RangeRequest getRequest;
        RangeResponse getResponse;
        
        getRequest.set_key(key);
        getRequest.set_range_end("\0", 1);
 
        Status getStatus = m_kvStub->Range(&context, getRequest , &getResponse);

        for(auto kv_item: getResponse.kvs()){
            pairs[kv_item.key()] = kv_item.value();
        }

        return getStatus;
    }     
    
    Status get(const std::string key, std::map<std::string, std::string> & pairs) {
        
        ClientContext context;
        RangeRequest getRequest;
        RangeResponse getResponse;
        
        getRequest.set_key(key);
        std::string range_end (key); 
        range_end.back()++;
        
        getRequest.set_range_end(range_end);
 
        Status getStatus = m_kvStub->Range(&context, getRequest , &getResponse);

        for(auto kv_item: getResponse.kvs()){
            pairs[kv_item.key()] = kv_item.value();
        }

        return getStatus;
    }
    
    Status getKeys(const std::string key, std::vector<std::string> & keys) {
        
        ClientContext context;
        RangeRequest getRequest;
        RangeResponse getResponse;
        
        getRequest.set_key(key);
        std::string range_end (key); 
        range_end.back()++;
        
        getRequest.set_range_end(range_end);
 
        Status getStatus = m_kvStub->Range(&context, getRequest , &getResponse);

        for(auto kv_item: getResponse.kvs()){
            keys.push_back(kv_item.key());
        }

        return getStatus;



    }

private:

    std::unique_ptr<KV::Stub> m_kvStub;
    std::unique_ptr<Watch::Stub> m_watchStub;
    std::unique_ptr<Lease::Stub> m_leaseStub;

};
#endif
