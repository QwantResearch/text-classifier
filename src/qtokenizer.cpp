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

#include "qtokenizer.h"

using namespace qnlp;

qtokenizer::qtokenizer(std::string &lang, bool lowercase) {
  this->set_tokenizer(lang, lowercase);
}

void qtokenizer::set_tokenizer(std::string &lang, bool lowercase) {
  _lang = lang;

  if (_lang == "fr") {
    _tokenizer = qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN, lowercase, false, false, false);
  } else if (_lang == "en") {
    _tokenizer = qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN, lowercase, false, false, false);
  } else {
    _tokenizer = qnlp::Tokenizer(qnlp::Tokenizer::PLAIN, lowercase, false, false, false);
  }
}

std::vector<std::string> qtokenizer::tokenize(std::string &input) {
  return _tokenizer->tokenize_sentence(input);
}

std::string qtokenizer::tokenize_str(std::string &input) {
  return _tokenizer->tokenize_sentence_to_string(input);
}
