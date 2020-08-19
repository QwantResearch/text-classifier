// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "tokenizer.h"

tokenizer::tokenizer(std::string &lang, bool lowercase) {
  this->set_tokenizer(lang, lowercase);
}

void tokenizer::set_tokenizer(std::string &lang, bool lowercase) {
  _lang = lang;

  if (_lang == "fr") {
    _tokenizer = new qnlp::Tokenizer_fr(lowercase, false, false, false);
  } else if (_lang == "en") {
    _tokenizer = new qnlp::Tokenizer_en(lowercase, false, false, false);
  } else if (_lang == "char") {
    _tokenizer = new qnlp::Tokenizer_char(lowercase, false, false, false);
  } else {
    _tokenizer = new qnlp::Tokenizer(lowercase, false, false, false);
  }
}

std::vector<std::string> tokenizer::tokenize(std::string &input) {
  return _tokenizer->tokenize_sentence(input);
}

std::string tokenizer::tokenize_str(std::string &input) {
  return _tokenizer->tokenize_sentence_to_string(input);
}
