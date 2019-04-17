// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "qtokenizer.h"

using namespace qnlp;

qtokenizer::qtokenizer(std::string& lang, bool lowercase)
{
    this->set_tokenizer(lang, lowercase);
}

void qtokenizer::set_tokenizer(std::string& lang, bool lowercase)
{
    _lang = lang;

    if (_lang == "fr") {
        _tokenizer = new qnlp::Tokenizer_fr(
            qnlp::Tokenizer::PLAIN, lowercase, false, false, false);
    } else if (_lang == "en") {
        _tokenizer = new qnlp::Tokenizer_en(
            qnlp::Tokenizer::PLAIN, lowercase, false, false, false);
    } else {
        _tokenizer = new qnlp::Tokenizer(
            qnlp::Tokenizer::PLAIN, lowercase, false, false, false);
    }
}

std::vector<std::string>
qtokenizer::tokenize(std::string& input)
{
    return _tokenizer->tokenize_sentence(input);
}

std::string
qtokenizer::tokenize_str(std::string& input)
{
    return _tokenizer->tokenize_sentence_to_string(input);
}
