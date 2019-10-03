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

#include <grpcpp/grpcpp.h>
#include <grpc/grpc.h>
#include "grpc_classification.grpc.pb.h"
#include "grpc_classification.pb.h"

#include "classifier_controller.h"
#include "tokenizer.h"
#include "abstract_server.h"
#include "utils.h"
#include "grpc_route_classify_impl.h"

using namespace std;
using namespace nlohmann;


class GrpcServer : public AbstractServer {//, public RouteClassify::Service {

public:
    using AbstractServer::AbstractServer;
    ~GrpcServer() {delete _service;}
    void Init(size_t thr = 2) override;
    void Start() override;
    void Shutdown() override;

private:
    GrpcRouteClassifyImpl* _service;
};

#endif // __GRPC_SERVER_H
