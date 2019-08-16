// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "grpc_server.h"

grpc::Status RouteClassifyImpl::GetDomains(grpc::ServerContext* context, 
                                           const Empty* request, 
                                           Domains* response) {

    for (auto& it: _server->getListClassifs()){
      response->add_domains(it->getDomain());
    }
    return grpc::Status::OK;
}

grpc::Status RouteClassifyImpl:: GetClassif(grpc::ServerContext* context,
                                           const TextToClassify* request,
                                           TextClassified* response) {

      // TODO: add debug logs

      std::string text = request->text();
      std::string domain = request->domain();
      std::string language = request->language();

      tokenizer l_tok(language, true);
      std::string tokenized = l_tok.tokenize_str(text);

      response->set_text(text);
      response->set_language(language);
      response->set_domain(domain);
      response->set_tokenized(tokenized);
      response->set_count(request->count());
      response->set_threshold(request->threshold());

      std::vector<std::pair<fasttext::real, std::string>> results;
      results = _server->askClassification(tokenized, domain, request->count(), request->threshold());

      for (auto& it: results){
        Score *score = response->add_intention();
        score->set_label(it.second);
        score->set_confidence(it.first);
        // TODO: do we need to delete Score?
      }

      return grpc::Status::OK;
}

grpc::Status RouteClassifyImpl::RouteClassify(grpc::ServerContext* context,
                                              grpc::ServerReaderWriter< TextClassified, 
                                              TextToClassify>* stream) {
    // TODO: Implement if needed
    return grpc::Status::OK;
}

RouteClassifyImpl::RouteClassifyImpl(grpc_server *server){
  _server = server;
}

void grpc_server::init(size_t thr){ //TODO: Check how to use thread number in grpc
  _service = new RouteClassifyImpl(this);
  ProcessCongifFile(_classif_config, _list_classifs);
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
  // TODO
}

