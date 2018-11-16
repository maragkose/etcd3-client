#include "Client.hpp"
#include "Condition.hpp"
#include "Request.hpp"
#include "Watcher.hpp"

#include <thread>

void receive(Watcher * oAsyncWatcher){

      oAsyncWatcher->recv([&](auto resp)
        {
            for(auto event: resp.events()){
                std::cout << 
                event.kv().key() <<  ":" <<  
                event.kv().value()<< std::endl;
            }
        });
}

int main() {
    
    Watcher oAsyncWatcher("localhost:2379");

    //
    // Setup watches and callbacks 
    //
    
    std::cerr << "Setup 1st watch" << std::endl;
    oAsyncWatcher.async_watch("SomeKey", [](WatchResponse &res) {
        std::cerr << "Key1 changed!" <<std::endl;
    }); 
    
    std::cerr << "Setup 2nd watch" << std::endl;
    oAsyncWatcher.async_watch("OtherKey", [](WatchResponse &res) { 
      std::cerr << "Key2 changed!" <<std::endl;
    }); 
 
    
    //
    // Setup receving of watcher events
    //
    std::thread watch_thread = std::thread([&](){ 
        receive(&oAsyncWatcher);    
    });
    
    //
    // Run watcher loop and join thread
    //
    oAsyncWatcher.run();
    watch_thread.join();
     
}
