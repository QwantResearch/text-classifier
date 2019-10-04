#include <boost/test/unit_test.hpp>
#include "classifier.h"

BOOST_AUTO_TEST_CASE( classifier_load_model_test )
// Test possible errors and success when loading model
{
    std::string filename;
    std::string domain("language_identification");
    std::string lang("");
    Classifier* classifier = nullptr;

    // Failed model loading because of file not found
    filename = "filename";
    try {
        classifier = new Classifier(filename, domain, lang);
    } catch(const std::invalid_argument &e) {
        BOOST_TEST(e.what() == filename + " cannot be opened for loading!");
    }
    BOOST_TEST(classifier == nullptr);

    // Failed model loading because of invalid format
    filename = "/opt/text-classifier/models_config.yaml";
    classifier = nullptr;
    try {
        classifier = new Classifier(filename, domain, lang);
    } catch(const std::invalid_argument &e) {
        BOOST_TEST(e.what() == filename + " has wrong file format!");
    }
    BOOST_TEST(classifier == nullptr);

    // Loaded model successfully
    filename = "/opt/text-classifier/resources/lid.176.ftz";
    classifier = nullptr;
    classifier = new Classifier(filename, domain, lang);
    BOOST_TEST(classifier != nullptr);

    // Loading same model twice is also successful
    Classifier* classifier2 = nullptr;
    classifier2 = new Classifier(filename, domain, lang);
    BOOST_TEST(classifier2 != nullptr);

    delete classifier, classifier2;
}

BOOST_AUTO_TEST_CASE( classifier_predict_test )
// Test number of results depending on count and threshold
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::string lang("");
    
    unique_ptr<Classifier> classifier;
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));
    
    std::string text("");
    std::string tokenized;
    std::vector<std::pair<fasttext::real, std::string>> result;

    // Test number of results depending on count parameter,
    // threshold set to default: 0.0
    result = classifier->Predict(text, tokenized, 5);
    BOOST_TEST(result.size() == 5);

    result = classifier->Predict(text, tokenized, 1);
    BOOST_TEST(result.size() == 1);

    result = classifier->Predict(text, tokenized, 10);
    BOOST_TEST(result.size() == 10);


    // Test number of results depending on threshold parameter
    result = classifier->Predict(text, tokenized, 10, 0.0);
    BOOST_TEST(result.size() == 10);

    result = classifier->Predict(text, tokenized, 10, 2.0);
    BOOST_TEST(result.size() == 0);
}

BOOST_AUTO_TEST_CASE( classifier_tokenization_test )
// Test Classifier tokenization
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::unique_ptr<Classifier> classifier;

    std::string lang, text, tokenized;

    // Test fr tokenizer
    lang = "fr";
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));
    text = "Bonjour, je m'appelle Henry.";
    classifier->Predict(text, tokenized, 1);
    BOOST_TEST(tokenized == "bonjour , je m' appelle henry .\n");
    // TODO: should we keep this new line char ?

    // Test en tokenizer
    lang = "en";
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));
    text = "My name's Bond. James Bond.";
    classifier->Predict(text, tokenized, 1);
    BOOST_TEST(tokenized == "my name 's bond . james bond .\n");

    // Test default tokenizer
    lang = ""; // same for any language that is not supported
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));
    classifier->Predict(text, tokenized, 1);
    BOOST_TEST(tokenized == "my name ' s bond . james bond .\n");

    // Test empty text tokenizer
    lang = "";
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));
    text = "";
    classifier->Predict(text, tokenized, 1);
    BOOST_TEST(tokenized == "\n");
}

BOOST_AUTO_TEST_CASE( classifier_domain_test )
// Test classifier domain name
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::string lang("fr");
    std::unique_ptr<Classifier> classifier;
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));

    std::string model_domain = classifier->GetDomain();
    BOOST_TEST(model_domain == domain);

    domain = "";
    classifier = unique_ptr<Classifier>(new Classifier(filename, domain, lang));
    model_domain = classifier->GetDomain();
    BOOST_TEST(classifier->GetDomain() == domain);
}
