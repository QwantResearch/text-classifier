#include "grpc_server.h"

RouteClassifyImpl::RouteClassifyImpl(string &classif_config) {
    ProcessCongifFile(classif_config, _list_classifs);
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

      std::string text = request->text();
      std::string domain = request->domain();

      response->set_text(text);
      response->set_count(request->count());
      response->set_threshold(request->threshold());
      response->set_language(request->language());
      response->set_domain(domain);

      std::vector<std::pair<fasttext::real, std::string>> results;
      results = RouteClassifyImpl::askClassification(text, domain, request->count(), request->threshold());

      for (auto& it: results){

        std::cout << it.second;

        Score *score = response->add_intention();
        score->set_label(it.second);
        score->set_confidence(it.first);
      }

      return grpc::Status::OK;
}

grpc::Status RouteClassifyImpl::RouteClassify(grpc::ServerContext* context,
                                              grpc::ServerReaderWriter< TextClassified, 
                                              TextToClassify>* stream) {
    return grpc::Status::OK;
}

std::vector<std::pair<fasttext::real, std::string>>
RouteClassifyImpl::askClassification(std::string &text, std::string &domain,
                               int count, float threshold) {
  std::vector<std::pair<fasttext::real, std::string>> to_return;
  if ((int)text.size() > 0) {
    auto it_classif = std::find_if(_list_classifs.begin(), _list_classifs.end(),
                                   [&](classifier *l_classif) {
                                     return l_classif->getDomain() == domain;
                                   });
    if (it_classif != _list_classifs.end()) {
      to_return = (*it_classif)->prediction(text, count, threshold);
    }
  }
  return to_return;
}

grpc_server::grpc_server(int num_port, string &classif_config, int debug_mode){
  _server_address = "0.0.0.0:" + std::to_string(num_port);
  _debug_mode = debug_mode;
  _classif_config = classif_config;
}

void grpc_server::init(size_t thr){ //Check how to use thread number in grpc

  RouteClassifyImpl service(_classif_config);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(_server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << _server_address << std::endl;
  server->Wait();
}
