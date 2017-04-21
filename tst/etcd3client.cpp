#include "Client.hpp"
#include "Condition.hpp"
#include "Request.hpp"

#include <thread>

template<typename T>
void do_stuff(T events) {
    std::cout << "Do stuff called" << std::endl;
}

int main() {

    Client oClient("localhost:2379");

    
    oClient.put("NewKey", "etcd!!");
    
    std::cout << oClient.get("foo1") << std::endl;
    //oClient.put("foo1", "bar1");
    //oClient.put("foo2", "bar2");
    //oClient.put("foo3", "bar3");
    std::pair <std::string, std::string> pair;
    //oClient.put("foo3", "bar3", pair);
    //std::cout << "foo3 previous[key:value]: "<< pair.first << ":"<< pair.second << std::endl; 
    //std::cout << oClient.get("Hallo") <<std::endl;
   
     std::map<std::string, std::string>  res1, res2;
    std::vector<std::string> res3;
    
    //oClient.getFromKey("foo1", res1);
    //std::cout << "===================================="<<std::endl; 
    //for(auto item: res1){ std::cout << item.first << ":" << item.second << std::endl; }
    //std::cout << "===================================="<<std::endl; 
    
    //oClient.get("foo", res2);
    //for(auto item: res2){ std::cout << item.first << ":" << item.second << std::endl; }
    //std::cout << "===================================="<<std::endl; 
    
    //oClient.getKeys("foo", res3);
    //for(auto item: res3){ std::cout << item << std::endl; }
    //std::cout << "===================================="<<std::endl; 

    oClient.watch("NewKey", 
        [&](auto &stream, auto resp){ 
            std::cout << "Rcvd:" 
                      << resp.watch_id() 
                      << ","  
                      << resp.canceled()
                      <<std::endl;
            for(auto event: resp.events()){
                std::cout << 
                event.kv().key() <<  ":" <<  
                event.kv().value()<< std::endl;
            }
            
            oClient.watchCancel(stream, resp.watch_id());
            std::cout << "Canceled:" 
                      << resp.watch_id() 
                     << ","
                      << resp.canceled() << std::endl;
            stream->WritesDone();
            stream->Finish();
       } 
    );
  
    //std::thread t([&oClient]{ oClient.watch("NewThread",  
    //    [](auto e){ 
    //        do_stuff(e); 
    //    }); 
    //});
    //t.join();
 
    // Creating conditions/requests lists 
    std::list<Request> sreqs;
    std::list<Request> freqs;
    std::list<Condition> conditions;

    //Condition con1("create", "foo1", 91);
    Condition con1("value","=",  "foo1", "value1");
    //Condition con1("modify", ">", "foo1", 90);
    //Condition con1("version", "=", "foo1", 1130);
    conditions.push_back(con1);

    Request req1("get", "foo3", "value1");
    Request req2("get", "foo2", "value1");
    Request req3("put", "newfoo2", "value1");
    sreqs.push_back(req1);
    sreqs.push_back(req2);
    sreqs.push_back(req3);

    freqs.push_back(req1);

    //Status s = oClient.transaction(conditions, sreqs, freqs); 
   // if(s.ok()){
   //     std::cerr << "ok" << s.error_code() << ":" << s.error_message() << std::endl;
   // } else {
   //     std::cerr << s.error_code() << ":" << s.error_message() << std::endl;
  //  }
    
    // Getting the responses through a callback 
    Status ts = oClient.transaction(conditions, sreqs, freqs, [](auto r, bool isConditionSuccess) {
        std::cerr << "The transaction condition was :" << isConditionSuccess << std::endl; 
        for(auto resp: r.responses()){
           for(auto kvs_items: resp.response_range().kvs()){
                std::cerr << kvs_items.key() << 
                      ":" << kvs_items.value() << std::endl;
            }                
        }
    });

     
}
