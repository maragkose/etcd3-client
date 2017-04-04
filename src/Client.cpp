#include "Client.hpp"


int main() {

    Client oClient("localhost:2379");

    oClient.put("Hallo", "etcd!!");
    oClient.put("foo1", "bar1");
    oClient.put("foo2", "bar2");
    oClient.put("foo3", "bar3");
    
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

}
