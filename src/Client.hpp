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
using etcdserverpb::WatchRequest;
using etcdserverpb::WatchCreateRequest;
using etcdserverpb::WatchResponse;
using etcdserverpb::TxnRequest;
using etcdserverpb::TxnResponse;
using grpc::Status;
using grpc::CompletionQueue;
using grpc::ClientAsyncReaderWriter;

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

    void updateLease(uint64_t leaseId) {

        ClientContext context;
        etcdserverpb::LeaseKeepAliveRequest leaseKeepAliveRequest;
        etcdserverpb::LeaseKeepAliveResponse leaseKeepAliveResponse;


        auto stream = m_leaseStub->LeaseKeepAlive(&context);

        leaseKeepAliveRequest.set_id(leaseId);
        stream->Write(leaseKeepAliveRequest);
        stream->Read(&leaseKeepAliveResponse);
    }

    Status createLease(uint64_t leaseId, uint64_t ttl) {

        ClientContext context;
        etcdserverpb::LeaseGrantRequest createLeaseRequest;
        etcdserverpb::LeaseGrantResponse createLeaseResponse;

        createLeaseRequest.set_id(leaseId);
        createLeaseRequest.set_ttl(ttl);

        Status createLeaseStatus = m_leaseStub->LeaseGrant(&context, createLeaseRequest, &createLeaseResponse);

        return createLeaseStatus;
    }

    template <typename T>
    void watch(const std::string key, T callback) {

        ClientContext context;
        WatchRequest watchRequest;
        CompletionQueue cq_;

        WatchResponse watchResponse;

        watchRequest.mutable_create_request()->set_key(key);
        auto stream = m_watchStub->Watch(&context);

        stream->Write(watchRequest);
        stream->Read(&watchResponse);

        while(true){

            bool op = stream->Read(&watchResponse);

            // event is of type:  mvccpb::Event
            for(auto event: watchResponse.events()){
                std::cerr << "Reveived watch_response event.type() = "<< event.type() << std::endl;
                callback(event);
            }

            if(!op){
                std::cerr << "Lost connection with stream..."<< std::endl;
                break;
            }
        }
    }

    Status txn(const std::string key, const std::string value) {

        ClientContext context;
        TxnRequest txnRequest;
        TxnResponse txnResponse;

        //  RangeRequest getRequest;
        std::unique_ptr<RangeRequest> getRequest(new RangeRequest());

        // add compare object to Txn
        auto compare = txnRequest.add_compare();
        compare->set_result(etcdserverpb::Compare_CompareResult_EQUAL);
        compare->set_target(etcdserverpb::Compare_CompareTarget_VALUE);
        compare->set_key(key);
        compare->set_value(value);

        //add success object to Txn
        auto success = txnRequest.add_success();

        getRequest->set_key(key);
        success->set_allocated_request_range(getRequest.release());
        Status getStatus = m_kvStub->Txn(&context, txnRequest , &txnResponse);

        for(auto resp: txnResponse.responses()){
            for(auto kvs_items: resp.response_range().kvs()){
                std::cerr << kvs_items.key() <<
                      ":" << kvs_items.value() << std::endl;
            }
        }
        return getStatus;
    }

private:

    std::unique_ptr<KV::Stub> m_kvStub;
    std::unique_ptr<Watch::Stub> m_watchStub;
    std::unique_ptr<Lease::Stub> m_leaseStub;

};
#endif
