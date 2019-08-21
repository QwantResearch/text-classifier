// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "grpc_route_classify_impl.h"

grpc::Status GrpcRouteClassifyImpl::GetDomains(grpc::ServerContext* context,
                                               const Empty* request,
                                               Domains* response) {

  for (auto& it: _classifier_controller->getListClassifs()) {
    response->add_domains(it->getDomain());
  }
  return grpc::Status::OK;
}

grpc::Status GrpcRouteClassifyImpl::GetClassif(grpc::ServerContext* context,
                                               const TextToClassify* request,
                                               TextClassified* response) {

  // TODO: add debug logs

  PrepareOutput(request, response);

  std::string tokenized = response->tokenized();
  std::string domain = response->domain();

  std::vector<std::pair<fasttext::real, std::string>> results;
  results = _classifier_controller->askClassification(tokenized, domain, response->count(), response->threshold());

  for (auto& it: results) {
    Score *score = response->add_intention();
    score->set_label(it.second);
    score->set_confidence(it.first);
  }

  return grpc::Status::OK;
}

grpc::Status GrpcRouteClassifyImpl::StreamClassify(grpc::ServerContext* context,
                                                   grpc::ServerReaderWriter< TextClassified,
                                                                            TextToClassify>* stream) {
  TextToClassify request;
  while (stream->Read(&request)) {
    TextClassified response;
    PrepareOutput(&request, &response);

    std::string tokenized = response.tokenized();
    std::string domain = response.domain();

    std::vector<std::pair<fasttext::real, std::string>> results;
    results = _classifier_controller->askClassification(tokenized, domain, response.count(), response.threshold());

    for (auto& it: results) {
      Score *score = response.add_intention();
      score->set_label(it.second);
      score->set_confidence(it.first);
    }

    stream->Write(response);
  }

  return grpc::Status::OK;
}

void GrpcRouteClassifyImpl::PrepareOutput(const TextToClassify* request, TextClassified* response) {
  std::string text = request->text();
  std::string language = request->language();

  tokenizer l_tok(language, true);
  std::string tokenized = l_tok.tokenize_str(text);

  response->set_text(request->text());
  response->set_language(request->language());
  response->set_domain(request->domain());
  response->set_count(request->count());
  response->set_threshold(request->threshold());
  response->set_tokenized(tokenized);
}

GrpcRouteClassifyImpl::GrpcRouteClassifyImpl(shared_ptr<ClassifierController> classifier_controller) {
  _classifier_controller = classifier_controller;
}