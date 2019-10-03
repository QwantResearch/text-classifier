// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "classifier.h"

std::vector<std::pair<fasttext::real, std::string>>
Classifier::Predict(std::string& text, std::string& tokenized, int count, float threshold) {
  std::vector<std::pair<fasttext::real, std::string>> results;
  if (*(text.end() - 1) != '\n')
    text.push_back('\n');
  tokenized=_tokenizer->TokenizeStr(text);

  std::stringstream istr(tokenized);
  _model.predictLine(istr, results, count, threshold);

  for (auto& r : results) {
    // FastText returns labels like : '__label__XX'
    r.second = r.second.substr(9);
  }
  return results;
}
