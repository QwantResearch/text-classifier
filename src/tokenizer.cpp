// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "tokenizer.h"

Tokenizer::Tokenizer(std::string& lang, bool lowercase) {
  this->SetTokenizer(lang, lowercase);
}

void Tokenizer::SetTokenizer(std::string& lang, bool lowercase) {
  _lang = lang;

  if (_lang == "fr") {
    _tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN, lowercase,
                                        false, false, false);
  } else if (_lang == "en") {
    _tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN, lowercase,
                                        false, false, false);
  } else {
    _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN, lowercase, false,
                                     false, false);
  }
}

std::vector<std::string> Tokenizer::Tokenize(std::string& input) {
  return _tokenizer->tokenize_sentence(input);
}

std::string Tokenizer::TokenizeStr(std::string& input) {
  return _tokenizer->tokenize_sentence_to_string(input);
}
