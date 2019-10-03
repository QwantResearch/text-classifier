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

  PrepareOutput(request, response);

  std::string text = response->text();
  std::string domain = response->domain();
  std::string tokenized;

  std::vector<std::pair<fasttext::real, std::string>> results;
  results = _classifier_controller->AskClassification(text, tokenized, domain, response->count(), response->threshold());
  response->set_tokenized(tokenized);

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
    PrepareOutput(&request, &response);

    std::string text = response.text();
    std::string domain = response.domain();
    std::string tokenized;

    std::vector<std::pair<fasttext::real, std::string>> results;
    results = _classifier_controller->AskClassification(text, tokenized, domain, response.count(), response.threshold());
    response.set_tokenized(tokenized);

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

void GrpcRouteClassifyImpl::PrepareOutput(const TextToClassify* request, TextClassified* response) {
  std::string text = request->text();

  response->set_text(request->text());
  response->set_domain(request->domain());
  response->set_count(request->count());
  response->set_threshold(request->threshold());
}

GrpcRouteClassifyImpl::GrpcRouteClassifyImpl(shared_ptr<ClassifierController> classifier_controller, int debug_mode) {
  _classifier_controller = classifier_controller;
  _debug_mode = debug_mode;
}