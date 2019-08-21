// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "grpc_server.h"

void grpc_server::init(size_t thr){ //TODO: Check how to use thread number in grpc
  _service = new GrpcRouteClassifyImpl(_classifier_controller, _debug_mode);
}

void grpc_server::start(){
  std::string server_address = "0.0.0.0:" + std::to_string(_num_port);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

void grpc_server::shutdown(){
  // TODO: /!\ can't shutdown from same thread
}

