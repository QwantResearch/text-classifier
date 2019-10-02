#include <boost/test/unit_test.hpp>
#include "classifier.h"

BOOST_AUTO_TEST_CASE( classifier_load_model_test )
// Test possible errors and success when loading model
{
    std::string filename;
    std::string domain("language_identification");
    std::string lang("");
    classifier* model = nullptr;

    // Failed model loading because of file not found
    filename = "filename";
    try {
        model = new classifier(filename, domain, lang);
    } catch(const std::invalid_argument &e) {
        BOOST_TEST(e.what() == filename + " cannot be opened for loading!");
    }
    BOOST_TEST(model == nullptr);

    // Failed model loading because of invalid format
    filename = "/opt/text-classifier/models_config.yaml";
    model = nullptr;
    try {
        model = new classifier(filename, domain, lang);
    } catch(const std::invalid_argument &e) {
        BOOST_TEST(e.what() == filename + " has wrong file format!");
    }
    BOOST_TEST(model == nullptr);

    // Loaded model successfully
    filename = "/opt/text-classifier/resources/lid.176.ftz";
    model = nullptr;
    model = new classifier(filename, domain, lang);
    BOOST_TEST(model != nullptr);

    // Loading same model twice is also successful
    classifier* model2 = nullptr;
    model2 = new classifier(filename, domain, lang);
    BOOST_TEST(model2 != nullptr);

    delete model, model2;
}

BOOST_AUTO_TEST_CASE( classifier_prediction_test )
// Test number of results depending on count and threshold
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::string lang("");
    
    unique_ptr<classifier> model;
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));
    
    std::string text("");
    std::string tokenized;
    std::vector<std::pair<fasttext::real, std::string>> result;

    // Test number of results depending on count parameter,
    // threshold set to default: 0.0
    result = model->prediction(text, tokenized, 5);
    BOOST_TEST(result.size() == 5);

    result = model->prediction(text, tokenized, 1);
    BOOST_TEST(result.size() == 1);

    result = model->prediction(text, tokenized, 10);
    BOOST_TEST(result.size() == 10);


    // Test number of results depending on threshold parameter
    result = model->prediction(text, tokenized, 10, 0.0);
    BOOST_TEST(result.size() == 10);

    result = model->prediction(text, tokenized, 10, 2.0);
    BOOST_TEST(result.size() == 0);
}

BOOST_AUTO_TEST_CASE( classifier_tokenization_test )
// Test classifier tokenization
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::unique_ptr<classifier> model;

    std::string lang, text, tokenized;

    // Test fr tokenizer
    lang = "fr";
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));
    text = "Bonjour, je m'appelle Henry.";
    model->prediction(text, tokenized, 1);
    BOOST_TEST(tokenized == "bonjour , je m' appelle henry .\n");
    // TODO: should we keep this new line char ?

    // Test en tokenizer
    lang = "en";
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));
    text = "Hello, I don't like my name, Jonas.";
    model->prediction(text, tokenized, 1);
    BOOST_TEST(tokenized == "hello , i do n't like my name , jonas .\n");

    // Test general tokenizer
    lang = ""; // any lang that is not supported is treated as fr
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));
    text = "Hello, I don't like my name, Jonas.";
    model->prediction(text, tokenized, 1);
    BOOST_TEST(tokenized == "hello , i don ' t like my name , jonas .\n");

    // Test empty text tokenizer
    lang = "";
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));
    text = "";
    model->prediction(text, tokenized, 1);
    BOOST_TEST(tokenized == "\n");
}

BOOST_AUTO_TEST_CASE( classifier_domain_test )
// Test classifier domain name
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::string lang("fr");
    std::unique_ptr<classifier> model;
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));

    std::string model_domain = model->getDomain();
    BOOST_TEST(model_domain == domain);

    domain = "";
    model = unique_ptr<classifier>(new classifier(filename, domain, lang));
    model_domain = model->getDomain();
    BOOST_TEST(model->getDomain() == domain);
}
