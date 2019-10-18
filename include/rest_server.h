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

class RestServer : public AbstractServer {

public:
  using AbstractServer::AbstractServer;
  ~RestServer(){}
  void Init(size_t thr = 2) override;
  void Start() override;
  void StartThreaded();
  void Shutdown() override;

private:
  std::unique_ptr<Http::Endpoint> _http_endpoint;
  Rest::Router _router;

  void SetupRoutes();

  void DoClassificationGet(const Rest::Request& request,
                           Http::ResponseWriter response);

  void DoClassificationPost(const Rest::Request& request,
                            Http::ResponseWriter response);

  void DoClassificationBatchPost(const Rest::Request& request,
                                 Http::ResponseWriter response);

  void FetchParamWithDefault(const json& j,
                              string& domain,
                              int& count,
                              float& threshold,
                              bool& debugmode);

  bool Process_localization(string& input, json& output);

  void WriteLog(string text_to_log) {}

  void DoAuth(const Rest::Request& request, Http::ResponseWriter response);
};

#endif // __REST_SERVER_H
