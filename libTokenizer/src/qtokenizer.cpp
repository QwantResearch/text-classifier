#include <iostream>

// #include <boost/program_options.hpp>
// #include <chrono>
// #include <algorithm>
#include "qnlp/tokenizer.h"
#include "qnlp/en_tokenizer.h"
#include "qnlp/fr_tokenizer.h"
// #include "bpe.h"
// #include "BatchReader.h"
// #include "BatchWriter.h"

// namespace po = boost::program_options;

#include <pybind11/pybind11.h>

using namespace pybind11;
using namespace std;
using namespace qnlp;

class qtokenizer
{
    public:
        qnlp::Tokenizer * _tokenizer;
        qnlp::Tokenizer_fr * _fr_tokenizer;
        qnlp::Tokenizer_en * _en_tokenizer;
        string _lang;
        qtokenizer(string &lang)
        {
            _lang = lang;
            if (lang == "fr")
            {
                _fr_tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN,false,false,false,false);
            }
            else if (lang == "en")
            {
                _en_tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN,false,false,false,false);
            }
            else
            {
                _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN,false,false,false,false);
            }
        }
        void set_tokenizer(string &lang)
        {
            _lang = lang;
            if (_lang == "fr")
            {
                _fr_tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN,false,false,false,false);
            }
            else if (_lang == "en")
            {
                _en_tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN,false,false,false,false);
            }
            else
            {
                _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN,false,false,false,false);
            }
        }
        vector<string> tokenize(string &input)
        {
            vector<string> to_return;
            if (_lang == "fr")
            {
                to_return=_fr_tokenizer->tokenize_sentence(input);
            }
            else if (_lang == "en")
            {
                to_return=_en_tokenizer->tokenize_sentence(input);
            }
            else
            {
                to_return=_tokenizer->tokenize_sentence(input);
            }
            return to_return;
        }
        string tokenize_str(string &input)
        {
            if (_lang == "fr")
            {
                return _fr_tokenizer->tokenize_sentence_to_string(input);
            }
            else if (_lang == "en")
            {
                return _en_tokenizer->tokenize_sentence_to_string(input);
            }
            else
            {
                return _tokenizer->tokenize_sentence_to_string(input);
            }
        }
};


PYBIND11_MODULE(qtokenizer, t) 
{
    pybind11::class_<qtokenizer>(t, "qtokenizer")
        .def(pybind11::init<std::string&>())
        .def("tokenize", &qtokenizer::tokenize)
        .def("tokenize_str", &qtokenizer::tokenize_str);
}


