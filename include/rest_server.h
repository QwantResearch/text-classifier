// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __REST_SERVER_H
#define __REST_SERVER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <pistache/client.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <sstream>
#include <time.h>
#include "yaml-cpp/yaml.h"

#include "classifier.h"
#include "tokenizer.h"

using namespace std;
using namespace nlohmann;
using namespace Pistache;

class rest_server {

public:
  rest_server(Address addr, string &classif_config, int debug_mode = 0);

  void init(size_t thr = 2);
  void start();
  void shutdown() { httpEndpoint->shutdown(); }

private:
  int _debug_mode;
  std::vector<classifier *> _list_classifs;
  std::shared_ptr<Http::Endpoint> httpEndpoint;
  Rest::Router router;
  typedef std::mutex Lock;
  typedef std::lock_guard<Lock> Guard;
  Lock nluLock;

  void setupRoutes();

  void doClassificationGet(const Rest::Request &request,
                           Http::ResponseWriter response);

  void doClassificationPost(const Rest::Request &request,
                            Http::ResponseWriter response);

  std::vector<std::pair<fasttext::real, std::string>>
  askClassification(std::string &text, std::string &domain, int count, float threshold);

  bool process_localization(string &input, json &output);

  void writeLog(string text_to_log) {}

  void doAuth(const Rest::Request &request, Http::ResponseWriter response);
};

#endif // __REST_SERVER_H
