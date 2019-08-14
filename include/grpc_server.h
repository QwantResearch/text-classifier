// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __GRPC_SERVER_H
#define __GRPC_SERVER_H

#include <algorithm>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <sstream>
#include <time.h>
#include "yaml-cpp/yaml.h"

#include <grpcpp/grpcpp.h>
#include <grpc/grpc.h>
#include "grpc_classification.grpc.pb.h"
#include "grpc_classification.pb.h"

#include "classifier.h"
#include "tokenizer.h"

using namespace std;
using namespace nlohmann;

class RouteClassifyImpl final : public RouteClassify::Service {
public:
  RouteClassifyImpl();

  grpc::Status GetDomains(grpc::ServerContext* context,
                          const Empty* request,
                          Domains* response) override;
  grpc::Status GetClassif(grpc::ServerContext* context,
                          const TextToClassify* request,
                          TextClassified* response) override;
  grpc::Status RouteClassify(grpc::ServerContext* context,
                             grpc::ServerReaderWriter< TextClassified, TextToClassify>* stream) override;
};

class grpc_server {

public:
  grpc_server(grpc::Channel addr, string &classif_config, int debug_mode = 0);

  void init(size_t thr = 2);
  void start();
  void shutdown() {}

private:
  int _debug_mode;
  std::vector<classifier *> _list_classifs;
  std::vector<std::pair<fasttext::real, std::string>>
  askClassification(std::string &text, std::string &domain, int count, float threshold);

  void writeLog(string text_to_log) { }
};

void RunGRPCServer();

#endif // __GRPC_SERVER_H
