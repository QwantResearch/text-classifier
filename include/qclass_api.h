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

#ifndef __QCLASS_API_H
#define __QCLASS_API_H

#include <algorithm>

#include "qtokenizer.h"
#include "qclassifier.h"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <pistache/client.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <sstream>
#include <time.h>


using namespace std;
using namespace nlohmann;
using namespace Pistache;

void printCookies(const Http::Request &req);
void Split(const std::string &line, std::vector<std::string> &pieces, const std::string del);

namespace Generic {
void handleReady(const Rest::Request &, Http::ResponseWriter response);
} // namespace Generic

const std::string currentDateTime();


class qclass_api {
  
public:
  qclass_api(Address addr, string& classif_config, int debug_mode = 0);
 
  void init(size_t thr);
  void start();
  void shutdown() { httpEndpoint->shutdown(); }

private:
  std::vector<qclassifier *> _list_classifs;
  FastText clang;
  FastText cIoT;
  FastText cint;
  FastText cshop;
  int _debug_mode;

  void setupRoutes();

  void doClassificationGet(const Rest::Request &request,
                           Http::ResponseWriter response);

  void doClassificationDomainsGet(const Rest::Request &request,
                                  Http::ResponseWriter response);
  
  void doClassificationPost(const Rest::Request &request,
                            Http::ResponseWriter response);

  std::vector<std::pair<fasttext::real, std::string>>
  askClassification(std::string &text, std::string &domain, int count);

  bool process_localization(string &input, json &output);

  void writeLog(string text_to_log) {}

  void doAuth(const Rest::Request &request, Http::ResponseWriter response);

  typedef std::mutex Lock;
  typedef std::lock_guard<Lock> Guard;
  Lock nluLock;
  std::shared_ptr<Http::Endpoint> httpEndpoint;
  Rest::Router router;
};

#endif // __QCLASS_API_H
