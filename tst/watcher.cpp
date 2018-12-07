#include "Client.hpp"
#include "Condition.hpp"
#include "Request.hpp"
#include "Watcher.hpp"

#include <thread>

int main() {
    
    Watcher oAsyncWatcher("localhost:2379");

    //
    // Setup watches and callbacks 
    //
    
    std::cerr << "Setup 1st watch" << std::endl;
    oAsyncWatcher.async_watch("SomeKey", [](WatchResponse &res) {
        std::cerr << "Key1 changed!" <<std::endl;
        for(auto event: res.events()){
            std::cout << 
            event.kv().key() <<  ":" <<  
            event.kv().value()<< std::endl;
        }
    }); 
    
    std::cerr << "Setup 2nd watch" << std::endl;
    oAsyncWatcher.async_watch("OtherKey", [](WatchResponse &res) { 
      std::cerr << "Key2 changed!" << std::endl;
      for(auto event: res.events()){
          std::cout << 
          event.kv().key() <<  ":" <<  
          event.kv().value()<< std::endl;
      }
    }); 
 
    //
    // Run watcher loop (blocks)
    //
    oAsyncWatcher.run();
}
