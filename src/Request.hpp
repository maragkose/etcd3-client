#ifndef _REQUEST_HPP
#define _REQUEST_HPP

#include "rpc.grpc.pb.h"

using etcdserverpb::RangeRequest;
using etcdserverpb::PutRequest;
using etcdserverpb::DeleteRangeRequest;

class Request {

public:

    Request(std::string request, std::string key, std::string val) :
        m_request(request),
        m_key(key), 
        m_value(val) 
    {
    }
    Request (std::string request,std::string key):
        m_request(request), 
        m_key(key) 
    {
    }
    ~Request(){} 

    void populate(auto success_failure){

        if(request_case() == "get"){
            std::unique_ptr<RangeRequest> getRequest(new RangeRequest());
            getRequest->set_key(key());
            success_failure->set_allocated_request_range(getRequest.release());
        } else
        if(request_case() == "put"){
            std::unique_ptr<PutRequest> putRequest(new PutRequest());
            putRequest->set_key(key());
            putRequest->set_value(value());
            success_failure->set_allocated_request_put(putRequest.release());
        } else
        if(request_case() == "del"){
            std::unique_ptr<DeleteRangeRequest> deleteRequest(new DeleteRangeRequest());
            deleteRequest->set_key(key());
            success_failure->set_allocated_request_delete_range(deleteRequest.release());
        }
    }

    std::string key() {
        return m_key;
    }
    std::string value() {
        return m_value;
    }
    std::string request_case() {
        return m_request;
    }

private:

    auto put_request() {
            std::unique_ptr<PutRequest> putRequest(new PutRequest());
            return putRequest.release();
    } 
    auto get_request() {
            std::unique_ptr<RangeRequest> getRequest(new RangeRequest());
            return getRequest.release();
    }       
    auto del_request() {
            std::unique_ptr<DeleteRangeRequest> delRequest(new DeleteRangeRequest());
            return delRequest.release();
    }       
    
    std::string m_request;
    std::string m_key;
    std::string m_value;
};
#endif
