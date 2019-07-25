// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __CLASSIFIER_H
#define __CLASSIFIER_H

#include <fasttext/fasttext.h>
#include "tokenizer.h"
#include <sstream>

class classifier {
public:
  classifier(std::string &filename, std::string &domain, std::string &lang) : _domain(domain) {
    _model.loadModel(filename.c_str());
    _tokenizer=new tokenizer(lang,true);
  }
  ~classifier(){delete(_tokenizer);};
  std::vector<std::pair<fasttext::real, std::string>>
  prediction(std::__cxx11::string &text, std::__cxx11::string &tokenized, int count, float threshold=0.0);
  std::string getDomain() { return _domain; }

private:
  std::string _domain;
  fasttext::FastText _model;
  tokenizer * _tokenizer;
};

#endif // __CLASSIFIER_H
