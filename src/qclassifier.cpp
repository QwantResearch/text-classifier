// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "katanoisi/qclassifier.h"

std::vector<std::pair<fasttext::real, std::string>>
qclassifier::prediction(std::string& text, int count)
{
    std::vector<std::pair<fasttext::real, std::string>> results;

    if (*(text.end()-1) != '\n')
      text.push_back('\n');

    std::stringstream istr(text);
    _model.predictLine(istr, results, count, 0.0);

    for (auto& r : results) {
        r.first = exp(r.first);
        // FastText returns labels like : '__label__XX'
        r.second = r.second.substr(9);
    }

    return results;
}
