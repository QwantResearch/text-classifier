// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "qclassifier.h"

std::vector<std::pair<fasttext::real, std::string>>
qclassifier::prediction(std::string& text, int count)
{
    std::vector<std::pair<fasttext::real, std::string>> results;

    // @Christophe: is this needed by FastText?
    // @Noel: yes, it's to avoid a well known bug, which is still not resolved.
    // This does not ensure that a '\n' is not present with text end
    int s_it = text.rfind('\n');
    if (s_it != (int)text.size() - 1)
        text.push_back('\n');

    //   std::istringstream istr(std::string(text));
    //   std::istream istr(std::string(text).c_str());
    //   std::istream istr();
    std::stringstream istr(text);
    _model.predictLine(istr, results, count, 0.0);

    for (auto& r : results) {
        r.first = exp(r.first);
        // FastText returns labels like : '__label__XX'
        r.second = r.second.substr(9);
    }

    return results;
}
