// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "grpc_route_classify_impl.h"

grpc::Status GrpcRouteClassifyImpl::GetDomains(grpc::ServerContext* context,
                                               const Empty* request,
                                               Domains* response) {

  for (auto& it: _classifier_controller->GetListClassifs()) {
    response->add_domains(it->GetDomain());
  }
  if (_debug_mode)
    cerr << "[DEBUG]: " << CurrentDateTime() << "\t" << "GetDomains" << endl;
  return grpc::Status::OK;
}

grpc::Status GrpcRouteClassifyImpl::GetClassif(grpc::ServerContext* context,
                                               const TextToClassify* request,
                                               TextClassified* response) {

  if (_debug_mode) {
    cerr << "[DEBUG]: " << CurrentDateTime() << "\t" << "GetClassif";
    cerr << "\t" << request->text() << "\t";
  }

  grpc::Status status = ParseInput(request, response);
  if (!status.ok()) {
    return status;
  }

  std::string text = response->text();
  std::string domain = response->domain();
  std::string tokenized;

  std::vector<std::pair<fasttext::real, std::string>> results;
  results = _classifier_controller->AskClassification(text, tokenized, domain, response->count(), response->threshold());
  response->set_tokenized(tokenized);
  
  if (results.size() == 1 && results.at(0).second == "DOMAIN ERROR"){
    return grpc::Status(grpc::StatusCode::NOT_FOUND, "domain not found");
  }
  if (_debug_mode)
    cerr << "Labels:";
  for (auto& it: results) {
    Score* score = response->add_intention();
    score->set_label(it.second);
    score->set_confidence(it.first);
    if (_debug_mode)
      cerr << " (" << score->label() << ", " << score->confidence() << ")";
  }
  if (_debug_mode)
    cerr << endl;

  return grpc::Status::OK;
}

grpc::Status GrpcRouteClassifyImpl::StreamClassify(grpc::ServerContext* context,
                                                   grpc::ServerReaderWriter< TextClassified,
                                                                            TextToClassify>* stream) {
  TextToClassify request;
  while (stream->Read(&request)) {

    if (_debug_mode) {
      cerr << "[DEBUG]: " << CurrentDateTime() << "\t" << "StreamClassify";
      cerr  << "\t" << request.text() << "\t";
    }

    TextClassified response;
    grpc::Status status = ParseInput(&request, &response);
    if (!status.ok()) {
      return status;
    }

    std::string text = response.text();
    std::string domain = response.domain();
    std::string tokenized;

    std::vector<std::pair<fasttext::real, std::string>> results;
    results = _classifier_controller->AskClassification(text, tokenized, domain, response.count(), response.threshold());
    response.set_tokenized(tokenized);

    if (results.size() == 1 && results.at(0).second == "DOMAIN ERROR"){
      return grpc::Status(grpc::StatusCode::NOT_FOUND, "domain not found");
    }

    if (_debug_mode)
      cerr << "Labels:";
    for (auto& it: results) {
      Score* score = response.add_intention();
      score->set_label(it.second);
      score->set_confidence(it.first);
      if (_debug_mode)
        cerr << " (" << score->label() << ", " << score->confidence() << ")";
    }
    if (_debug_mode)
      cerr << endl;

    stream->Write(response);
  }

  return grpc::Status::OK;
}

grpc::Status GrpcRouteClassifyImpl::ParseInput(const TextToClassify* request, TextClassified* response) {
  if (request->text() == "")
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "text value must be set");
  if (request->domain() == "")
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "domain value must be set");
  if (request->count() <= 0)
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "count value must be set and >= 1");
  
  response->set_text(request->text());
  response->set_domain(request->domain());
  response->set_count(request->count());
  response->set_threshold(request->threshold());

  return grpc::Status::OK;
}

GrpcRouteClassifyImpl::GrpcRouteClassifyImpl(shared_ptr<ClassifierController> classifier_controller, int debug_mode) {
  _classifier_controller = classifier_controller;
  _debug_mode = debug_mode;
  grpc::EnableDefaultHealthCheckService(true);
}