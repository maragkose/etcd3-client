#ifndef WATCHER_HPP
#define WATHCER_HPP

#include "rpc.grpc.pb.h"
#include <typeinfo>
#include <grpc++/grpc++.h>
#include <grpc/support/log.h>
#include <sstream>
using grpc::Channel;
using grpc::ClientContext;
using etcdserverpb::KV;
using etcdserverpb::Watch;
using etcdserverpb::WatchRequest;
using etcdserverpb::WatchCreateRequest;
using etcdserverpb::WatchResponse;
using grpc::Status;
using grpc::CompletionQueue;
using grpc::ClientAsyncReaderWriter;

class Watcher {

public:

    explicit Watcher(std::shared_ptr<Channel> channel)
    :
      m_Channel(channel),
      m_watch_stub(Watch::NewStub(channel))
    {
    }

    explicit Watcher(std::string address)
    : Watcher(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()))
    {
    }

    // TODO: Run calls all read requests and then another recv will have to sconsume the completion queue
    void run() {
        for(auto &pair: m_container) {
            std::cerr << "Request Read for tag: "<< pair.first << std::endl;
            AsyncWatcherCall* watcher_call = static_cast<AsyncWatcherCall*>(pair.first);
            watcher_call->m_stream->Read(&(watcher_call->m_watchResponse), pair.first);
        }
        loop_cq();
    }
    void loop_cq() {
        void * got_tag;
        bool ok = false;
        while(m_completionQueue.Next(&got_tag, &ok)) {
            if(ok == false){
                return;
            } else {
                    if(got_tag != (void*)2 && got_tag != (void*)1) {
                        AsyncWatcherCall* watcher_call = static_cast<AsyncWatcherCall*>(got_tag);

                        std::cerr << "Status for received tag ["<< got_tag << "]: " << watcher_call->m_status.ok() << std::endl; 
                        
                        auto cb = m_container[watcher_call->get_tag()];
                        cb(watcher_call->m_watchResponse);
                        watcher_call->m_stream->Read(&(watcher_call->m_watchResponse), got_tag);
                    }
            }
        } // end while   
    }
    
    template <typename T>
    void recv(T callback) {
        
        void * got_tag;
        bool ok = false;
       
        while(m_completionQueue.Next(&got_tag, &ok)) {
            if(ok == false){
                break;
            } else {
                if(got_tag != (void*)2 && got_tag != (void*)1) {
                    AsyncWatcherCall* watcher_call = static_cast<AsyncWatcherCall*>(got_tag);
                    std::cerr << "Status for received tag ["
                              << got_tag << "]:" 
                              << watcher_call->m_status.ok() 
                              << std::endl; 
                    callback(watcher_call->m_watchResponse);
                }
            }
        } // end while   
    }

    void async_watch(const std::string& key, std::function<void(WatchResponse &res)> cb) {
            
        AsyncWatcherCall* watch_call = new AsyncWatcherCall(key, &m_completionQueue, this);
        
        watch_call->send();
        std::cerr << "Adding callback to map with tag: " << watch_call->get_tag() << std::endl;
        m_container[watch_call->get_tag()] = cb;
    }

    bool cancel(const int64_t watch_id) {

        WatchRequest watchRequest;
        WatchResponse watchResponse;

        watchRequest.mutable_cancel_request()->set_watch_id(watch_id);
        return true;
    }

    void stop(){
        //m_stream->WritesDone();
        //m_stream->Finish();
    }

private:
    
    CompletionQueue                     m_completionQueue;
    std::shared_ptr<Channel>            m_Channel;
    std::unique_ptr<Watch::Stub>         m_watch_stub;
    std::map<void *, std::function<void (WatchResponse &response)>>  m_container;
    int m_id;

    class AsyncWatcherCall {

    public:

        AsyncWatcherCall(const std::string &key, CompletionQueue *cq, Watcher *watcher) :
            m_key(key), 
            m_cq(cq),
            m_watcher(watcher)
        {
            void *got_tag;
            bool ok;
            auto async_stream = m_watcher->m_watch_stub->AsyncWatch(&m_context, m_cq, (void*)2);
            m_stream = std::move(async_stream);
            auto status = m_cq->Next(&got_tag, &ok);
            m_tag = static_cast<void*>(this);
           // std::stringstream ss;
           // ss << address;  
           // m_tag = ss.str();
            std::cerr << "Ctor of AsyncWatcherCall ["<<m_tag<<"]"<< std::endl;
        }

       std::string get_id(){
            //return "";//std::to_string(m_watchResponse.watch_id());
            return std::to_string(m_watchResponse.watch_id());
       }
        
        void send() {
                WatchRequest watchRequest;
                watchRequest.mutable_create_request()->set_key(m_key);
          
                bool ok= false;
                void * got_tag;
                m_stream->Write(watchRequest, (void *)1);
                auto status = m_cq->Next(&got_tag, &ok);
                std::cerr << "Write ok and got_tag" << got_tag << ":" << status<<std::endl;
        }
        void * get_tag() {
            return m_tag;
        }

//private:
    
        ClientContext           m_context;
        void *                  m_tag;
        std::string             m_key;
        WatchResponse           m_watchResponse;
        Status                  m_status;
        CompletionQueue         *m_cq;
        Watcher                 *m_watcher;
        std::unique_ptr< ::grpc::ClientAsyncReaderWriter< ::etcdserverpb::WatchRequest, ::etcdserverpb::WatchResponse>> m_stream;
    };
};
#endif
