//#include "grpc_server.h"

// RouteClassifyImpl::RouteClassifyImpl(const std::string& db) {
//   // RouteClassify::ParseDb(db, &feature_list_);
//   std::cout << "hello" << std::endl;
//   std::cout << db << std::endl;
// }

// grpc::Status RouteClassifyImpl::GetDomains(grpc::ServerContext* context, const Empty* request, Domains* response) {
//     response->add_domains("intent_tmp");
//     return grpc::Status::OK;
// }

// void RunGRPCServer() {
//   std::string server_address("0.0.0.0:9010");
//   RouteClassifyImpl service();

//   grpc::ServerBuilder builder;
//   builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
//   // builder.RegisterService(&service);
//   // std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
//   // std::cout << "Server listening on " << server_address << std::endl;
//   // server->Wait();
// }

// #include <grpc/grpc.h>
// #include <grpcpp/grpcpp.h>
// // Which one to put ?
// #include "grpc_classification.grpc.pb.h"
// #include "grpc_classification.pb.h"

#include "grpc_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using grpc_classification::HelloRequest;
using grpc_classification::HelloReply;
using grpc_classification::Greeter;

class GreeterServiceImpl final : public Greeter::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloReply* reply) override {
    std::string prefix("Hello ");
    reply->set_message(prefix + request->name());
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  GreeterServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

// int main(int argc, char** argv) {
//   RunServer();

//   return 0;
// }