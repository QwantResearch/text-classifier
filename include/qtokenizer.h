// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#ifndef __QTOKENIZER_H
#define __QTOKENIZER_H

#include <iostream>

#include "qnlp/tokenizer.h"
#include "qnlp/fr_tokenizer.h"
#include "qnlp/en_tokenizer.h"

using namespace qnlp;

class qtokenizer {
public:
  qtokenizer(std::string &lang, bool lowercase = true);

  void set_tokenizer(std::string &lang, bool lowercase = true);

  std::vector<std::string> tokenize(std::string &input);
  std::string tokenize_str(std::string &input);

private:
  std::string _lang;
  qnlp::Tokenizer* _tokenizer;
};

#endif // __QTOKENIZER_H
