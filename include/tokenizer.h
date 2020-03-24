// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include <iostream>

#include "en_tokenizer.h"
#include "fr_tokenizer.h"
#include "tokenizer.h"

class Tokenizer {
public:
  Tokenizer(std::string& lang, bool lowercase = true);
  ~Tokenizer(){delete(_tokenizer);};
  void SetTokenizer(std::string& lang, bool lowercase = true);

  std::vector<std::string> Tokenize(std::string& input);
  std::string TokenizeStr(std::string& input);

private:
  std::string _lang;
  qnlp::Tokenizer* _tokenizer;
};

#endif // __TOKENIZER_H
