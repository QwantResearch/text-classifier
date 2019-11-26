#include <boost/test/unit_test.hpp>
#include "grpc_route_classify_impl.h"

BOOST_AUTO_TEST_CASE( grpc_route_classify_impl_init_test )
// Test success and failure when starting GrpcRouteClassifyImpl
{
    std::string model_config_file("/opt/text-classifier/tests/models_config.yaml");
    std::shared_ptr<ClassifierController> classifier_controller = make_shared<ClassifierController>(model_config_file);

    GrpcRouteClassifyImpl grpc_route_classify_impl(classifier_controller, 0);
}

BOOST_AUTO_TEST_CASE( grpc_route_classify_impl_get_domains_test )
// Test GetDomains
{
    std::string model_config_file("/opt/text-classifier/tests/models_config.yaml");
    std::shared_ptr<ClassifierController> classifier_controller = make_shared<ClassifierController>(model_config_file);

    GrpcRouteClassifyImpl grpc_route_classify_impl(classifier_controller, 0);

    
    grpc::ServerContext* context = new grpc::ServerContext();
    grpc::Status status;


    const Empty* request_get_domains = new Empty();
    Domains response_get_domains;
    status = grpc_route_classify_impl.GetDomains(context, request_get_domains, &response_get_domains);
    BOOST_TEST(status.error_code() == grpc::StatusCode::OK);
    BOOST_TEST(response_get_domains.domains_size() == 1);
    BOOST_TEST(response_get_domains.domains().at(0) == std::string("language_identification"));
}

BOOST_AUTO_TEST_CASE( grpc_route_classify_impl_get_classif_test )
// Test GetClassif
{
    std::string model_config_file("/opt/text-classifier/tests/models_config.yaml");
    std::shared_ptr<ClassifierController> classifier_controller = make_shared<ClassifierController>(model_config_file);

    GrpcRouteClassifyImpl grpc_route_classify_impl(classifier_controller, 0);

    
    grpc::ServerContext* context = new grpc::ServerContext();
    grpc::Status status;


    TextToClassify* request_get_classif = new TextToClassify();
    TextClassified response_get_classif;

    // Test failure because of text missing
    status = grpc_route_classify_impl.GetClassif(context, request_get_classif, &response_get_classif);
    BOOST_TEST(status.error_code() == grpc::StatusCode::INVALID_ARGUMENT);
    BOOST_TEST(status.error_message() == "text value must be set");


    request_get_classif->set_text(std::string("Bonjour, je m'appelle Henry."));

    // Test failure because of domain missing
    status = grpc_route_classify_impl.GetClassif(context, request_get_classif, &response_get_classif);
    BOOST_TEST(status.error_code() == grpc::StatusCode::INVALID_ARGUMENT);
    BOOST_TEST(status.error_message() == "domain value must be set");

    // Test failure because of count value missing or == 0
    request_get_classif->set_domain(std::string("fake_domain"));
    status = grpc_route_classify_impl.GetClassif(context, request_get_classif, &response_get_classif);
    BOOST_TEST(status.error_code() == grpc::StatusCode::INVALID_ARGUMENT);
    BOOST_TEST(status.error_message() == "count value must be set and >= 1");
    
    request_get_classif->set_count(0);status = grpc_route_classify_impl.GetClassif(context, request_get_classif, &response_get_classif);
    BOOST_TEST(status.error_code() == grpc::StatusCode::INVALID_ARGUMENT);
    BOOST_TEST(status.error_message() == "count value must be set and >= 1");

    // Test failure because of domain not found
    request_get_classif->set_domain(std::string("fake_domain"));
    request_get_classif->set_count(1);
    status = grpc_route_classify_impl.GetClassif(context, request_get_classif, &response_get_classif);
    BOOST_TEST(status.error_code() == grpc::StatusCode::NOT_FOUND);
    BOOST_TEST(status.error_message() == "domain not found");

    // Test success
    request_get_classif->set_domain(std::string("language_identification"));
    request_get_classif->set_count(10);
    request_get_classif->set_threshold(0.0);
    status = grpc_route_classify_impl.GetClassif(context, request_get_classif, &response_get_classif);
    BOOST_TEST(status.error_code() == grpc::StatusCode::OK);
    BOOST_TEST(response_get_classif.intention_size() == 10);
}

// TODO: Test StreamClassify, but it requires ServerReaderWriter, and its constructor is private
