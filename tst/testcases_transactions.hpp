SCENARIO ("simple transaction: condition value equal", "[transactions]"){

    GIVEN("A key-value pair [myKey:myValue] exists and a condition myKey=myValue"){
        Client oClient("localhost:2379");
        oClient.put("MyKey", "MyValue");
            
        std::list<Request> sreqs;
        std::list<Request> freqs;
        std::list<Condition> conditions;

        // Create a condition on value equals 
        Condition con1("value","=",  "MyKey", "MyValue");
        conditions.push_back(con1);

        // create requests 
        Request req1("get", "MyKey", "MyValue");
        Request req2("put", "MyNewKey", "MyNewValue");
        Request req3("put", "MyNewKeyFail", "MyNewValueFail");
       
        // add requests on the success list (condition success) 
        sreqs.push_back(req1);
        sreqs.push_back(req2);

        // add one request on the failure list (condition failure) 
        freqs.push_back(req1);
        freqs.push_back(req3);
   
        WHEN("A transaction with a condition on VALUE is created that expects MyKey EQUALS myVALUE") {

            THEN("The sucess list of the requests is executed"){
                // Getting the responses of the transaction using  a callback 
                Status ts = oClient.transaction(conditions, sreqs, freqs, [](auto r, bool isConditionSuccess) {
                    std::cerr << "The transaction condition was :" << isConditionSuccess << std::endl;
                    REQUIRE(isConditionSuccess == true); 
                    for(auto resp: r.responses()){
                       for(auto kvs_items: resp.response_range().kvs()){
                            std::cerr << kvs_items.key() << 
                                  ":" << kvs_items.value() << std::endl;
                            REQUIRE(kvs_items.key()=="MyKey"); 
                            REQUIRE(kvs_items.value()=="MyValue"); 
                        }                
                    }
                });
                REQUIRE(oClient.get("MyNewKey")=="MyNewValue");
            }
        }
        WHEN("myKey:myValue changes and value equals to myValue2"){
            
            oClient.put("MyKey", "MyValueChanged");
            
            THEN("The transaction condition fails and the failure list of request is executed"){
                Status ts = oClient.transaction(conditions, sreqs, freqs, [](auto r, bool isConditionSuccess) {
                    REQUIRE(isConditionSuccess == false); 
                    for(auto resp: r.responses()){
                       for(auto kvs_items: resp.response_range().kvs()){
                            REQUIRE(kvs_items.key()=="MyKey"); 
                            REQUIRE(kvs_items.value()=="MyValueChanged"); 
                        }                
                    }
                });
                REQUIRE(oClient.get("MyNewKeyFail")=="MyNewValueFail");
            }
        } 
    }
}
