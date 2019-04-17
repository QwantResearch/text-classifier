// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

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


class qclass_server {
  
public:
  qclass_server(Address addr, string& classif_config, int debug_mode = 0);
 
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
