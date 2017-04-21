#ifndef TESTCASES_PUT
#define TESTCASES_PUT

TEST_CASE ("Simple kv put", "[put]"){
    Client oClient("localhost:2379");
    Status status = oClient.put("NewKey", "etcd!!");
    REQUIRE ( status.ok() == true );
}


TEST_CASE ("put kv get prev kv in a pair", "[put]"){
    Client oClient("localhost:2379");
    std::pair <std::string, std::string> pair;
    Status status = oClient.put("foo3", "bar3", pair);
    std::cout << "foo3 previous[key:value]: "<< pair.first << ":"<< pair.second << std::endl;
    REQUIRE ( status.ok() == true );
}
#endif
