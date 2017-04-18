#include "Client.hpp"


int main() {

    Client oClient("localhost:2379");

    std::pair <std::string, std::string> pair;
    
    oClient.put("Hallo", "etcd!!");
    oClient.put("foo1", "bar1");
    oClient.put("foo2", "bar2");
    oClient.put("foo3", "bar3");
    oClient.put("foo3", "bar3", pair);
    std::cout << "foo3 previous[key:value]: "<< pair.first << ":"<< pair.second << std::endl; 
    std::cout << oClient.get("Hallo") <<std::endl;
   
     std::map<std::string, std::string>  res1, res2;
    std::vector<std::string> res3;
    
    oClient.getFromKey("foo1", res1);
    std::cout << "===================================="<<std::endl; 
    for(auto item: res1){ std::cout << item.first << ":" << item.second << std::endl; }
    std::cout << "===================================="<<std::endl; 
    
    oClient.get("foo", res2);
    for(auto item: res2){ std::cout << item.first << ":" << item.second << std::endl; }
    std::cout << "===================================="<<std::endl; 
    
    oClient.getKeys("foo", res3);
    for(auto item: res3){ std::cout << item << std::endl; }
    std::cout << "===================================="<<std::endl; 

    Status s = oClient.txn("foo", "There8");
    if(s.ok()){
        std::cerr << s.error_code() << ":" << s.error_message() << std::endl;
    } else {
        std::cerr << s.error_code() << ":" << s.error_message() << std::endl;
    }

    oClient.watch("foo", 
        [](auto event){ std::cout << "Rcvd:" << event.type()  << ":" 
                                                              << event.kv().key() << "-"
                                                              << event.kv().value() 
                                                              << std::endl;
       } 
    );

}
