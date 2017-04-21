
TEST_CASE ("get kv", "[get]"){
    Client oClient("localhost:2379");
    auto result = oClient.get("foo3");
    REQUIRE (result == "bar3");
}
