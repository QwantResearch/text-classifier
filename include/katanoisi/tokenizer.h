// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include <iostream>

#include "qnlp/en_tokenizer.h"
#include "qnlp/fr_tokenizer.h"
#include "qnlp/tokenizer.h"

class tokenizer {
public:
  tokenizer(std::string &lang, bool lowercase = true);

  void set_tokenizer(std::string &lang, bool lowercase = true);

  std::vector<std::string> tokenize(std::string &input);
  std::string tokenize_str(std::string &input);

private:
  std::string _lang;
  qnlp::Tokenizer *_tokenizer;
};

#endif // __TOKENIZER_H
