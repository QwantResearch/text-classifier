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

#ifndef __QCLASSIFIER_H
#define __QCLASSIFIER_H

#include <fasttext/fasttext.h>
using namespace fasttext;


class qclassifier {
public:
  qclassifier(std::string &filename, std::string &domain) :
    _domain(domain) { _model.loadModel(filename.c_str()); }

  std::vector<std::pair<fasttext::real, std::string>> prediction(std::string text, int count);
  std::string getDomain() { return _domain; }

private:
  std::string _domain;
  fasttext::FastText _model;
};

#endif // __QCLASSIFIER_H

