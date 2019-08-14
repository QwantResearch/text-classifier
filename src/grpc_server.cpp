#include "grpc_server.h"

RouteClassifyImpl::RouteClassifyImpl() {
}

grpc::Status RouteClassifyImpl::GetDomains(grpc::ServerContext* context, 
                                           const Empty* request, 
                                           Domains* response) {
    response->add_domains("intent_tmp");
    return grpc::Status::OK;
}

grpc::Status RouteClassifyImpl:: GetClassif(grpc::ServerContext* context,
                                           const TextToClassify* request,
                                           TextClassified* response) {
      // Score *score();
      // score->label = "label";
      // score->confidence = 0.5;

      response->set_text(request->text());
      response->set_count(request->count());
      response->set_threshold(request->threshold());
      response->set_language(request->language());
      // response->add_intention(score);

      return grpc::Status::OK;
                        }

grpc::Status RouteClassifyImpl::RouteClassify(grpc::ServerContext* context,
                                              grpc::ServerReaderWriter< TextClassified, 
                                              TextToClassify>* stream) {
    return grpc::Status::OK;
                           }

void RunGRPCServer() {
  std::string server_address("0.0.0.0:9010");
  RouteClassifyImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}
