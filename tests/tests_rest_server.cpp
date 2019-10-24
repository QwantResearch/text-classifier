#include <boost/test/unit_test.hpp>
#include "rest_server.h"


#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include "httplib.h"

int num_port = 9797;


BOOST_AUTO_TEST_CASE( rest_server_test )
// Test all routes from route_classify
// TODO: split tests by route. But Starting multiple rest servers
// (one after the other) give "Address already in use"
{
    std::string model_config_file("/opt/text-classifier/tests/models_config.yaml");
    RestServer* rest_server = new RestServer(num_port, model_config_file, 0);
    rest_server->Init();
    rest_server->StartThreaded();

    httplib::Client client("localhost", num_port);


    // Test GET route
    // Test GET success
    auto response = client.Get("/intention");
    BOOST_TEST(response->body == std::string("{\"domains\":[\"language_identification\"]}"));
    BOOST_TEST(response->status == 200);


    // Test POST route
    // Test POST failure due to domain missing or invalid
    response = client.Post("/intention", "{}", "application/json");
    BOOST_TEST(response->body == std::string("`domain` value is null"));
    BOOST_TEST(response->status == 400);

    response = client.Post("/intention", "{\"text\":\"text\"}", "application/json");
    BOOST_TEST(response->body == std::string("`domain` value is null"));
    BOOST_TEST(response->status == 400);

    response = client.Post("/intention", "{\"text\":\"text\", \"domain\":\"wrong_domain_name\"}", "application/json");
    BOOST_TEST(response->body == std::string("`domain` value is not valid (wrong_domain_name)"));
    BOOST_TEST(response->status == 400);

    // Test POST failure due to text
    response = client.Post("/intention", "{\"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->body == std::string("The `text` value is required"));
    BOOST_TEST(response->status == 400);

    // Test POST success
    response = client.Post("/intention", "{\"text\":\"text\", \"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->status == 200);

    response = client.Post("/intention", "{\"text\":\"\", \"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->status == 200);
    

    // Test batch POST route
    // Test batch POST failure due to domain missing or invalid
    response = client.Post("/intention_batch", "{}", "application/json");
    BOOST_TEST(response->body == std::string("`domain` value is null"));
    BOOST_TEST(response->status == 400);

    response = client.Post("/intention_batch", "{\"batch_data\":[]}", "application/json");
    BOOST_TEST(response->body == std::string("`domain` value is null"));
    BOOST_TEST(response->status == 400);

    response = client.Post("/intention_batch", "{\"batch_data\":[{\"text\": \"text\"}], \"domain\":\"wrong_domain_name\"}", "application/json");
    BOOST_TEST(response->body == std::string("`domain` value is not valid (wrong_domain_name)"));
    BOOST_TEST(response->status == 400);

    // Test POST failure due to batch_data
    response = client.Post("/intention_batch", "{\"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->body == std::string("`batch_data` value is required"));
    BOOST_TEST(response->status == 400);

    response = client.Post("/intention_batch", "{\"batch_data\":[{}], \"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->body == std::string("`text` value is required for each item in `batch_data` array"));
    BOOST_TEST(response->status == 400);

    // Test POST success
    response = client.Post("/intention_batch", "{\"batch_data\":[{\"text\": \"text\"}], \"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->status == 200);

    response = client.Post("/intention_batch", "{\"batch_data\":[{\"text\": \"\"}], \"domain\":\"language_identification\"}", "application/json");
    BOOST_TEST(response->status == 200);


    rest_server->Shutdown();
    delete rest_server;
}
