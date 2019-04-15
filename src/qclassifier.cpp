/*
 * MIT License
 *
 * Copyright (c) 2019 Qwant Research
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 * Christophe Servan <c.servan@qwantresearch.com>
 * Noel Martin <n.martin@qwantresearch.com>
 *
 */

#include "qclassifier.h"

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
