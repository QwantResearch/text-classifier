#include <boost/test/unit_test.hpp>
#include "classifier_controller.h"

BOOST_AUTO_TEST_CASE( classifier_controller_init_test )
// Test possible errors and success when starting classifier_controller
{
    std::string model_config_file;

    // Test failure: file not found
    model_config_file = "";
    auto IsBadFileMessage = [&](const YAML::BadFile exception){
        BOOST_CHECK_EQUAL(exception.what(), std::string("yaml-cpp: error at line 0, column 0: bad file"));
        return true;
    };
    BOOST_CHECK_EXCEPTION(new ClassifierController(model_config_file), YAML::BadFile, IsBadFileMessage);

    // Test failure: wrong file format
    model_config_file = "/opt/text-classifier/tests/resources/wrong_format_model_config.yaml";
    auto IsBadFormatFileMessage = [&](const YAML::BadSubscript exception){
        BOOST_CHECK_EQUAL(exception.what(), std::string("yaml-cpp: error at line 0, column 0: operator[] call on a scalar"));
        return true;
    };
    BOOST_CHECK_EXCEPTION(new ClassifierController(model_config_file), YAML::BadSubscript, IsBadFormatFileMessage);

    // Test success
    model_config_file = "/opt/text-classifier/tests/models_config.yaml";
    ClassifierController classifier_controller = ClassifierController(model_config_file);
}

BOOST_AUTO_TEST_CASE( classifier_controller_process_config_file_test )
// Test config file processing
{
    std::string model_config_file("/opt/text-classifier/tests/models_config.yaml");
    // std::unique_ptr<ClassifierController> classifier_controller = std::unique_ptr<ClassifierController>(
    //     new ClassifierController(model_config_file));

    ClassifierController classifier_controller = ClassifierController(model_config_file);

    BOOST_TEST(classifier_controller.GetListClassifs().size() = 1);
    BOOST_TEST(classifier_controller.GetListClassifs().at(0)->GetDomain() == std::string("language_identification"));
}

BOOST_AUTO_TEST_CASE( classifier_controller_ask_classification_test )
// Test success and failure of AskClassification
{
    std::string model_config_file("/opt/text-classifier/tests/models_config.yaml");
    // std::unique_ptr<ClassifierController> classifier_controller = std::unique_ptr<ClassifierController>(
    //     new ClassifierController(model_config_file));

    ClassifierController classifier_controller = ClassifierController(model_config_file);

    std::string text("Bonjour, je m'appelle Henry.");
    std::string tokenized;
    std::vector<std::pair<fasttext::real, std::string>> results;

    // Failure due to domain error
    std::string domain("");
    results = classifier_controller.AskClassification(text, tokenized, domain, 10, 0.0);
    BOOST_TEST(results.size() == 1);
    BOOST_TEST(results.at(0).second == "DOMAIN ERROR");

    // Success
    domain = "language_identification";
    results = classifier_controller.AskClassification(text, tokenized, domain, 10, 0.0);
    BOOST_TEST(results.size() == 10);
    BOOST_TEST(results.at(0).second != "DOMAIN ERROR");
}