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

#include "classifier.h"
#include "tokenizer.h"
#include "abstract_server.h"

using namespace std;
using namespace nlohmann;
using namespace Pistache;

class rest_server : public AbstractServer {

public:
  using AbstractServer::AbstractServer;
  ~rest_server(){httpEndpoint->shutdown();}
  void init(size_t thr = 2) override;
  void start() override;
  void shutdown() override;

private:
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

  void doClassificationBatchPost(const Rest::Request &request,
                                 Http::ResponseWriter response);

  bool fetchParamWithDefault(const json& j,
                             string& domain,
                             int& count,
                             float& threshold,
                             bool& debugmode);

  bool process_localization(string &input, json &output);

  void writeLog(string text_to_log) {}

  void doAuth(const Rest::Request &request, Http::ResponseWriter response);
};

#endif // __REST_SERVER_H
