// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "rest_server.h"
#include "utils.h"

void rest_server::init(size_t thr) {
  Pistache::Port port(_num_port);
  Address addr(Ipv4::any(), port);
  httpEndpoint = std::make_shared<Http::Endpoint>(addr);

  auto opts = Http::Endpoint::options().threads(thr).flags(
      Tcp::Options::InstallSignalHandler);
  httpEndpoint->init(opts);
  setupRoutes();
}

void rest_server::start() {
  httpEndpoint->setHandler(router.handler());
  httpEndpoint->serve();
  httpEndpoint->shutdown();
}

void rest_server::setupRoutes() {
  using namespace Rest;

  Routes::Post(router, "/classify/",
               Routes::bind(&rest_server::doClassificationPost, this));

  Routes::Post(router, "/classify_batch/",
              Routes::bind(&rest_server::doClassificationBatchPost, this));

  Routes::Get(router, "/classify/",
              Routes::bind(&rest_server::doClassificationGet, this));

  // For backward compatibality
  Routes::Post(router, "/intention/",
               Routes::bind(&rest_server::doClassificationPost, this));

  Routes::Post(router, "/intention_batch/",
              Routes::bind(&rest_server::doClassificationBatchPost, this));

  Routes::Get(router, "/intention/",
              Routes::bind(&rest_server::doClassificationGet, this));
}

void rest_server::doClassificationGet(
  const Rest::Request &request,
  Http::ResponseWriter response
) {
  response.headers().add<Http::Header::AccessControlAllowHeaders>(
      "Content-Type");
  response.headers().add<Http::Header::AccessControlAllowMethods>(
      "GET, POST, DELETE, OPTIONS");
  response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
  response.headers().add<Http::Header::ContentType>(MIME(Application, Json));

  bool first_domain = true;
  string response_string = "{\"domains\":[";
  for (auto& it: _classifier_controller->getListClassifs()) {
    if (!first_domain)
      response_string.append(",");
    response_string.append("\"");
    response_string.append(it->getDomain());
    response_string.append("\"");
    first_domain = false;
  }
  response_string.append("]}");
  if (_debug_mode != 0)
    cerr << "[DEBUG]\t" << currentDateTime() << "\tRESPONSE\t" << response_string << endl;
  response.send(Pistache::Http::Code::Ok, response_string);
}

void rest_server::doClassificationPost(
  const Rest::Request &request,
  Http::ResponseWriter response
) {
  response.headers().add<Http::Header::AccessControlAllowHeaders>(
      "Content-Type");
  response.headers().add<Http::Header::AccessControlAllowMethods>(
      "GET, POST, DELETE, OPTIONS");
  response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
  nlohmann::json j = nlohmann::json::parse(request.body());
  
  int count;
  float threshold;
  bool debugmode;
  string domain;
  
  if (!rest_server::fetchParamWithDefault(j, domain, count, threshold, debugmode)) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request, "`domain` value is null");
  }

  if (j.find("text") != j.end()) 
  {
    string text = j["text"];
    string tokenized;
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << currentDateTime() << "\t" << "ASK CLASS :\t" << j << endl;
    std::vector<std::pair<fasttext::real, std::string>> results;
    results = _classifier_controller->askClassification(text, tokenized, domain, count, threshold);
    if ((int)results.size() > 0)
    {
        if ((int)results[0].second.compare("DOMAIN ERROR")==0)
        {
            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Bad_Request,std::string("`domain` value is not valid ("+domain+")"));
            cerr << "[ERROR]\t" << currentDateTime() << "\tRESPONSE\t" << "`domain` value is not valid ("+domain+")\t" << j << endl;
            return;
        }
    }
    j.push_back(nlohmann::json::object_t::value_type(string("tokenized"), tokenized));
    j.push_back(nlohmann::json::object_t::value_type(string("intention"), results));
    j.push_back(nlohmann::json::object_t::value_type(string("classification"), results));
    j.push_back(nlohmann::json::object_t::value_type(string("model_name"),
          _classifier_controller->getModelName(domain)));
    std::string s = j.dump();
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << currentDateTime() << "\tRESPONSE\t" << s << endl;
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Ok, std::string(s));
  } else {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request,std::string("The `text` value is required"));
  }
}

void rest_server::doClassificationBatchPost(
  const Rest::Request &request,
  Http::ResponseWriter response
) {
  response.headers().add<Http::Header::AccessControlAllowHeaders>(
      "Content-Type");
  response.headers().add<Http::Header::AccessControlAllowMethods>(
      "GET, POST, DELETE, OPTIONS");
  response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
  nlohmann::json j = nlohmann::json::parse(request.body());

  int count;
  float threshold;
  bool debugmode;
  string domain;
  
  if (!rest_server::fetchParamWithDefault(j, domain, count, threshold, debugmode)) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request, "`domain` value is null");
  }

  if (j.find("batch_data") != j.end()) {
    for (auto& it: j["batch_data"]){
      if (it.find("text") != it.end()) {
        string text = it["text"];
        string tokenized;
        if (_debug_mode != 0)
          cerr << "[DEBUG]\t" << currentDateTime() << "\tASK CLASS:\t" << it << endl;
        auto results = _classifier_controller->askClassification(text, tokenized, domain, count, threshold);
        if ((int)results.size() > 0)
        {
            if ((int)results[0].second.compare("DOMAIN ERROR")==0)
            {
                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Bad_Request,std::string("`domain` value is not valid ("+domain+")"));
                cerr << "[ERROR]\t" << currentDateTime() << "\tRESPONSE\t" << "`domain` value is not valid ("+domain+")\t" << j << endl;
                return;
            }
        }
        it.push_back(nlohmann::json::object_t::value_type(string("tokenized"), tokenized));
        it.push_back(nlohmann::json::object_t::value_type(string("intention"), results));
        it.push_back(nlohmann::json::object_t::value_type(string("classification"), results));
        it.push_back(nlohmann::json::object_t::value_type(string("model_name"),
          _classifier_controller->getModelName(domain)));
      } else {
        response.headers().add<Http::Header::ContentType>(
            MIME(Application, Json));
        response.send(Http::Code::Bad_Request,
                      std::string("`text` value is required for each item in `batch_data` array"));
      }
    }
    std::string s = j.dump();
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << currentDateTime() << "\tRESULT CLASS:\t" << s << endl;
    response.headers().add<Http::Header::ContentType>(
        MIME(Application, Json));
    response.send(Http::Code::Ok, std::string(s));
  } else {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request,
                  std::string("`batch_data` value is required"));
  }
}

bool rest_server::fetchParamWithDefault(
  const nlohmann::json& j,
  string& domain,
  int& count,
  float& threshold,
  bool& debugmode
) {
  count = 10;
  threshold = 0.0;
  debugmode = false;

  if (j.find("count") != j.end()) {
    count = j["count"];
  }
  if (j.find("threshold") != j.end()) {
    threshold = j["threshold"];
  }
  if (j.find("debug") != j.end()) {
    debugmode = j["debug"];
  }
  if (j.find("domain") != j.end()) {
    domain = j["domain"];
  } else {
    return false;
  }
  return true;
}

void rest_server::doAuth(const Rest::Request &request, Http::ResponseWriter response) {
  printCookies(request);
  response.cookies().add(Http::Cookie("lang", "fr-FR"));
  response.send(Http::Code::Ok);
}

void rest_server::shutdown() {
  httpEndpoint->shutdown();
}
