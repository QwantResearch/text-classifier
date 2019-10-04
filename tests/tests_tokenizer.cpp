#include <boost/test/unit_test.hpp>
#include "classifier.h"


BOOST_AUTO_TEST_CASE( tokenizer_init_test )
// Test success when initializing tokenizer
{
    std::string lang("");
    bool lowercase(false);
    Tokenizer* tokenizer = nullptr;

    // Loaded tokenizer successfully
    tokenizer = new Tokenizer(lang, lowercase);
    BOOST_TEST(tokenizer != nullptr);

    // Loading same tokenizer twice is also successful
    Tokenizer* tokenizer2 = nullptr;
    tokenizer2 = new Tokenizer(lang, lowercase);
    BOOST_TEST(tokenizer2 != nullptr);

    // Setting tokenizer successfully
    tokenizer->SetTokenizer(lang, lowercase);
    BOOST_TEST(tokenizer != nullptr);

    delete tokenizer, tokenizer2;
}

BOOST_AUTO_TEST_CASE( tokenizer_tokenize_str_test )
// Test string tokenization results
{
    std::string lang;
    std::string result_str;
    
    unique_ptr<Tokenizer> tokenizer;


    // Test tokenization in french (lowercased or not)
    std::string text("Bonjour, je m'appelle Henry.");
    lang = "fr";
    
    tokenizer = unique_ptr<Tokenizer>(new Tokenizer(lang, true));
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "bonjour , je m' appelle henry .");

    tokenizer->SetTokenizer(lang, false);
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "Bonjour , je m' appelle Henry .");

    
    // Test tokenization in english (lowercased or not)
    text = "My name's Bond. James Bond.";
    lang = "en";

    tokenizer->SetTokenizer(lang, true);
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "my name 's bond . james bond .");

    tokenizer->SetTokenizer(lang, false);
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "My name 's Bond . James Bond .");

    // Test tokenization in general (lowercased or not)
    lang = "";

    tokenizer->SetTokenizer(lang, true);
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "my name ' s bond . james bond .");

    tokenizer->SetTokenizer(lang, false);
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "My name ' s Bond . James Bond .");

    // Test empty text tokenization
    text = "";
    result_str = tokenizer->TokenizeStr(text);
    BOOST_TEST(result_str == "");

}

BOOST_AUTO_TEST_CASE( tokenizer_tokenize_vec_test )
// Test string tokenization results
{
    std::string lang;
    std::vector<std::string> result_vec;
    
    unique_ptr<Tokenizer> tokenizer;


    // Test tokenization in french (lowercased or not)
    std::string text("Bonjour, je m'appelle Henry.");
    lang = "fr";
    
    tokenizer = unique_ptr<Tokenizer>(new Tokenizer(lang, true));
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({"bonjour", ",", "je", "m'", "appelle", "henry", "."}));

    tokenizer->SetTokenizer(lang, false);
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({"Bonjour", ",", "je", "m'", "appelle", "Henry", "."}));

    
    // Test tokenization in english (lowercased or not)
    text = "My name's Bond. James Bond.";
    lang = "en";

    tokenizer->SetTokenizer(lang, true);
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({"my", "name", "'s", "bond", ".", "james", "bond", "."}));

    tokenizer->SetTokenizer(lang, false);
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({"My", "name", "'s", "Bond", ".", "James", "Bond", "."}));

    // Test default tokenization (lowercased or not)
    lang = "";

    tokenizer->SetTokenizer(lang, true);
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({"my", "name", "'", "s", "bond", ".", "james", "bond", "."}));

    tokenizer->SetTokenizer(lang, false);
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({"My", "name", "'", "s", "Bond", ".", "James", "Bond", "."}));

    // Test empty text tokenization
    text = "";
    result_vec = tokenizer->Tokenize(text);
    BOOST_TEST(result_vec == std::vector<std::string>({}));
}