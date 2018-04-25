#include <iostream>

// #include <boost/program_options.hpp>
// #include <chrono>
// #include <algorithm>
#include "tokenizer.h"
// #include "bpe.h"
// #include "BatchReader.h"
// #include "BatchWriter.h"

// namespace po = boost::program_options;

#include <pybind11/pybind11.h>

using namespace pybind11;
using namespace std;
using namespace webee;

class qtokenizer
{
    public:
        webee::Tokenizer * _tokenizer;
        string _lang;
        qtokenizer(string &src_lang)
        {
            _lang = src_lang;
            _tokenizer = new webee::Tokenizer(false,false,false,false,src_lang);
        }
        vector<string> tokenize(string &input)
        {
            vector<string> to_return=_tokenizer->tokenize_sentence(input);
            return to_return;
        }
        string tokenize_str(string &input)
        {
            // cerr << "INSIDE:" << input << endl;
            vector<string> to_return=_tokenizer->tokenize_sentence(input);
            // cerr << "INSIDE2:" << input << endl;
            stringstream l_out;
            l_out.str("");
            for (int i=0; i < (int)to_return.size(); i++)
            {
                if (i == (int)to_return.size()-1)
                {
                    l_out << to_return[i];
                }
                else
                {
                    l_out << to_return[i] << " ";
                }
            }
            return l_out.str();
        }
};


PYBIND11_MODULE(qtokenizer, t) 
{
    pybind11::class_<qtokenizer>(t, "qtokenizer")
        .def(pybind11::init<std::string&>())
        .def("tokenize", &qtokenizer::tokenize)
        .def("tokenize_str", &qtokenizer::tokenize_str);
}


