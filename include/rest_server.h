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

// class RouterHandlerTimeout : public Rest::RouterHandler {
  // void onTimeout(const Http::Request& request, Http::ResponseWriter writer) {
  //   std::cout << "HERE" << std::endl;
  //   writer.send(Http::Code::No_Content);
  // }
// };

class RouterTimeout;

class RouterHandlerTimeout : public Rest::Private::RouterHandler {
public:

  // RouterHandlerTimeout(const Pistache::Rest::Router& router) : 
  //   Rest::Private::RouterHandler(router) { }

  RouterHandlerTimeout(const Rest::Router& router) : 
      Rest::Private::RouterHandler(router) {}

  void onTimeout(const Http::Request& request, Http::ResponseWriter writer) override;
};

class RouterTimeout : public Rest::Router {
public:
  std::shared_ptr<RouterHandlerTimeout> handlerWithTimeout() const {
    return std::make_shared<RouterHandlerTimeout>(*this);
  }
};

struct PrintException {
    void operator()(std::exception_ptr exc) const {
        try {
            std::rethrow_exception(exc);
        } catch (const std::exception& e) {
            std::cerr << "An exception occured: " << e.what() << std::endl;
        }
    }
};

class MyHandler : public Http::Handler {

  HTTP_PROTOTYPE(MyHandler)

  void onRequest(
          const Http::Request& req,
          Http::ResponseWriter response) {

      if (req.resource() == "/intention") {
          if (req.method() == Http::Method::Get) {
            response.timeoutAfter(std::chrono::seconds(1));
            int i = 0;
            while(i < 3){
              sleep(1);
              i += 1;
            }
            response.send(Http::Code::Bad_Request, "PasTimeout");
          }
      }
  }

  void onTimeout(const Http::Request& req, Http::ResponseWriter response) {
      std::cout << "MyHandler::onTimeout" << std::endl;

      UNUSED(req);
      response
          .send(Http::Code::Request_Timeout, "Timeout")
          .then([=](ssize_t) { }, PrintException());
  }

};


class rest_server : public AbstractServer {

public:
  using AbstractServer::AbstractServer;
  ~rest_server(){httpEndpoint->shutdown();}
  void init(size_t thr = 2) override;
  void start() override;
  void shutdown() override;

private:
  std::shared_ptr<Http::Endpoint> httpEndpoint;
  // RouterTimeout router;
  std::shared_ptr<RouterHandlerTimeout> handler;
  // std::shared_ptr<Rest::Private::RouterHandler> handler;
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

  //void onTimeout(const Http::Request& request, Http::ResponseWriter writer) override;

  void fetchParamWithDefault(const json& j,
                              string& domain,
                              int& count,
                              float& threshold,
                              bool& debugmode);

  bool process_localization(string &input, json &output);

  void writeLog(string text_to_log) {}

  void doAuth(const Rest::Request &request, Http::ResponseWriter response);
};

#endif // __REST_SERVER_H
