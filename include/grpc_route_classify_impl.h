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

class GrpcRouteClassifyImpl : public RouteClassify::Service {
public:
    GrpcRouteClassifyImpl(ClassifierController *classifier_controller);
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

    ClassifierController *_classifier_controller;
};

#endif // __GRPC_ROUTE_CLASSIFY_IMPL_H