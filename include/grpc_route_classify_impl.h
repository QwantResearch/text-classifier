// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __GRPC_ROUTE_CLASSIFY_IMPL_H
#define __GRPC_ROUTE_CLASSIFY_IMPL_H

#include <grpcpp/grpcpp.h>
#include <grpc/grpc.h>
#include "grpc_classification.grpc.pb.h"
#include "grpc_classification.pb.h"

#include "tokenizer.h"
#include "classifier_controller.h"
#include "abstract_server.h"
#include "utils.h"

class GrpcRouteClassifyImpl : public RouteClassify::Service {
public:
    GrpcRouteClassifyImpl(shared_ptr<ClassifierController> classifier_controller, int debug_mode);
    ~GrpcRouteClassifyImpl() {};
private:
    grpc::Status GetDomains(grpc::ServerContext* context,
                            const Empty* request,
                            Domains* response) override;
    grpc::Status GetClassif(grpc::ServerContext* context,
                            const TextToClassify* request,
                            TextClassified* response) override;
    grpc::Status StreamClassify(grpc::ServerContext* context,
                                grpc::ServerReaderWriter< TextClassified, TextToClassify>* stream) override;

    void PrepareOutput(const TextToClassify* request, TextClassified* response);

    shared_ptr<ClassifierController> _classifier_controller;
    int _debug_mode;
};

#endif // __GRPC_ROUTE_CLASSIFY_IMPL_H