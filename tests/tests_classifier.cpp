#include <boost/test/unit_test.hpp>
#include "classifier.h"

BOOST_AUTO_TEST_CASE( classifier_test_function )
/* Compare with void classifier_test_function() */
{
    std::string filename("/opt/text-classifier/resources/lid.176.ftz");
    std::string domain("language_identification");
    std::string lang("");
    
    classifier* model = new classifier(filename, domain, lang);
    
    std::string text("bonjour");
    std::string tokenized;

    std::vector<std::pair<fasttext::real, std::string>> result;
    result = model->prediction(text, tokenized, 5);
    BOOST_TEST(result.size() == 5);


    result = model->prediction(text, tokenized, 1);
    BOOST_TEST(result.size() == 1);


    result = model->prediction(text, tokenized, 10);
    BOOST_TEST(result.size() == 10);

    result = model->prediction(text, tokenized, 10, 2.0);
    BOOST_TEST(result.size() == 0);
}