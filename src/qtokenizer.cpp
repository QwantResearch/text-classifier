/***************************************************************************
* MIT License
* 
* Copyright (c) 2018 Christophe SERVAN, Qwant Research, 
* email: christophe[dot]servan[at]qwantresearch[dot]com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
***************************************************************************/
#include "qtokenizer.h"
using namespace std;
using namespace qnlp;
qtokenizer::qtokenizer(string &lang)
{
    _lang = lang;
    if (lang == "fr")
    {
        _fr_tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN,true,false,false,false);
    }
    else if (lang == "en")
    {
        _en_tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN,true,false,false,false);
    }
    else
    {
        _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN,true,false,false,false);
    }
}
void qtokenizer::set_tokenizer(string &lang)
{
    _lang = lang;
    if (_lang == "fr")
    {
        _fr_tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN,true,false,false,false);
    }
    else if (_lang == "en")
    {
        _en_tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN,true,false,false,false);
    }
    else
    {
        _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN,true,false,false,false);
    }
}
qtokenizer::qtokenizer(string &lang, bool lowercase)
{
    _lang = lang;
    if (lang == "fr")
    {
        _fr_tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN,lowercase,false,false,false);
    }
    else if (lang == "en")
    {
        _en_tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN,lowercase,false,false,false);
    }
    else
    {
        _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN,lowercase,false,false,false);
    }
}
void qtokenizer::set_tokenizer(string &lang, bool lowercase)
{
    _lang = lang;
    if (_lang == "fr")
    {
        _fr_tokenizer = new qnlp::Tokenizer_fr(qnlp::Tokenizer::PLAIN,lowercase,false,false,false);
    }
    else if (_lang == "en")
    {
        _en_tokenizer = new qnlp::Tokenizer_en(qnlp::Tokenizer::PLAIN,lowercase,false,false,false);
    }
    else
    {
        _tokenizer = new qnlp::Tokenizer(qnlp::Tokenizer::PLAIN,lowercase,false,false,false);
    }
}
vector<string> qtokenizer::tokenize(string &input)
{
    vector<string> to_return;
    if (_lang == "fr")
    {
        to_return=_fr_tokenizer->tokenize_sentence(input);
    }
    else if (_lang == "en")
    {
        to_return=_en_tokenizer->tokenize_sentence(input);
    }
    else
    {
        to_return=_tokenizer->tokenize_sentence(input);
    }
    return to_return;
}
string qtokenizer::tokenize_str(string &input)
{
    if (_lang == "fr")
    {
        return _fr_tokenizer->tokenize_sentence_to_string(input);
    }
    else if (_lang == "en")
    {
        return _en_tokenizer->tokenize_sentence_to_string(input);
    }
    else
    {
        return _tokenizer->tokenize_sentence_to_string(input);
    }
}


