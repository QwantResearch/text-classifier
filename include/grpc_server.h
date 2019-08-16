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

#include "classifier.h"
#include "tokenizer.h"
#include "abstract_server.h"
#include "utils.h"

using namespace std;
using namespace nlohmann;


class RouteClassifyImpl;

class grpc_server : public AbstractServer {//, public RouteClassify::Service {

public:
    using AbstractServer::AbstractServer;
    void init(size_t thr = 2) override;
    void start() override;
    void shutdown() override;

private:
    RouteClassifyImpl *_service; // TODO: Inquiry the best way to solve this cross-reference (1)
};


class RouteClassifyImpl : public RouteClassify::Service {
public:
    RouteClassifyImpl(grpc_server *server);
private:
    grpc_server *_server; // TODO: Inquiry the best way to solve this cross-reference (2)

    grpc::Status GetDomains(grpc::ServerContext* context,
                            const Empty* request,
                            Domains* response) override;
    grpc::Status GetClassif(grpc::ServerContext* context,
                            const TextToClassify* request,
                            TextClassified* response) override;
    grpc::Status StreamClassify(grpc::ServerContext* context,
                                grpc::ServerReaderWriter< TextClassified, TextToClassify>* stream) override;

    void PrepareOutput(const TextToClassify* request, TextClassified* response);
};

#endif // __GRPC_SERVER_H
