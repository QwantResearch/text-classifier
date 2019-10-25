// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "rest_server.h"
#include "utils.h"

void RestServer::Init(size_t thr) {
  Pistache::Port port(_num_port);
  Address addr(Ipv4::any(), port);
  _http_endpoint = std::unique_ptr<Http::Endpoint>(new Http::Endpoint(addr));

  auto opts = Http::Endpoint::options().threads(thr);
  _http_endpoint->init(opts);
  SetupRoutes();
}

void RestServer::Start() {
  _http_endpoint->setHandler(_router.handler());
  _http_endpoint->serve();
}

void RestServer::StartThreaded() {
  _http_endpoint->setHandler(_router.handler());
  _http_endpoint->serveThreaded();
}

void RestServer::SetupRoutes() {
  using namespace Rest;

  Routes::Post(_router, "/intention/",
               Routes::bind(&RestServer::DoClassificationPost, this));

  Routes::Post(_router, "/intention_batch/",
              Routes::bind(&RestServer::DoClassificationBatchPost, this));

  Routes::Get(_router, "/intention/",
              Routes::bind(&RestServer::DoClassificationGet, this));

  Routes::Get(_router, "/status/",
              Routes::bind(&RestServer::GetStatus, this));
}

void RestServer::DoClassificationGet(const Rest::Request& request,
                                      Http::ResponseWriter response) {
  response.headers().add<Http::Header::AccessControlAllowHeaders>(
      "Content-Type");
  response.headers().add<Http::Header::AccessControlAllowMethods>(
      "GET, POST, DELETE, OPTIONS");
  response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
  response.headers().add<Http::Header::ContentType>(MIME(Application, Json));

  bool first_domain = true;
  string response_string = "{\"domains\":[";
  for (auto& it: _classifier_controller->GetListClassifs()){
    if (!first_domain)
      response_string.append(",");
    response_string.append("\"");
    response_string.append(it->GetDomain());
    response_string.append("\"");
    first_domain = false;
  }
  response_string.append("]}");
  if (_debug_mode != 0)
    cerr << "[DEBUG]\t" << CurrentDateTime() << "\tRESPONSE\t" << response_string << endl;
  response.send(Pistache::Http::Code::Ok, response_string);
}

void RestServer::DoClassificationPost(const Rest::Request& request,
                                       Http::ResponseWriter response) {
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
  
  try {
    RestServer::FetchParamWithDefault(j, domain, count, threshold, debugmode);
  } catch (std::runtime_error e) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request, e.what());
  }

  if (j.find("text") != j.end()) 
  {
    string text = j["text"];
    string tokenized;
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << CurrentDateTime() << "\t" << "ASK CLASS :\t" << j << endl;
    std::vector<std::pair<fasttext::real, std::string>> results;
    results = _classifier_controller->AskClassification(text, tokenized, domain, count, threshold);
    if ((int)results.size() > 0)
    {
        if ((int)results[0].second.compare("DOMAIN ERROR")==0)
        {
            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Bad_Request,std::string("`domain` value is not valid ("+domain+")"));
            cerr << "[ERROR]\t" << CurrentDateTime() << "\tRESPONSE\t" << "`domain` value is not valid ("+domain+")\t" << j << endl;
            return;
        }
    }
    j.push_back(nlohmann::json::object_t::value_type(string("tokenized"), tokenized));
    j.push_back(nlohmann::json::object_t::value_type(string("intention"), results));
    std::string s = j.dump();
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << CurrentDateTime() << "\tRESPONSE\t" << s << endl;
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Ok, std::string(s));
  } else {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request,std::string("The `text` value is required"));
  }
}

void RestServer::DoClassificationBatchPost(const Rest::Request& request,
                                       Http::ResponseWriter response) {
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
  
  try {
    RestServer::FetchParamWithDefault(j, domain, count, threshold, debugmode);
  } catch (std::runtime_error e) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request, e.what());
  }
  
  if (j.find("batch_data") != j.end()) {
    for (auto& it: j["batch_data"]){
      if (it.find("text") != it.end()) {
        string text = it["text"];
        string tokenized;
        if (_debug_mode != 0)
          cerr << "[DEBUG]\t" << CurrentDateTime() << "\tASK CLASS:\t" << it << endl;
        auto results = _classifier_controller->AskClassification(text, tokenized, domain, count, threshold);
        if ((int)results.size() > 0)
        {
            if ((int)results[0].second.compare("DOMAIN ERROR")==0)
            {
                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Bad_Request,std::string("`domain` value is not valid ("+domain+")"));
                cerr << "[ERROR]\t" << CurrentDateTime() << "\tRESPONSE\t" << "`domain` value is not valid ("+domain+")\t" << j << endl;
                return;
            }
        }
        it.push_back(nlohmann::json::object_t::value_type(string("tokenized"), tokenized));
        it.push_back(nlohmann::json::object_t::value_type(string("intention"), results));        
      } 
      else 
      {
        response.headers().add<Http::Header::ContentType>(
            MIME(Application, Json));
        response.send(Http::Code::Bad_Request,
                      std::string("`text` value is required for each item in `batch_data` array"));
      }
    }
    std::string s = j.dump();
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << CurrentDateTime() << "\tRESULT CLASS:\t" << s << endl;
    response.headers().add<Http::Header::ContentType>(
        MIME(Application, Json));
    response.send(Http::Code::Ok, std::string(s));
  } else {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request,
                  std::string("`batch_data` value is required"));
  }
}

void RestServer::FetchParamWithDefault(const nlohmann::json& j, 
                            string& domain, 
                            int& count,
                            float& threshold,
                            bool& debugmode){
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
    throw std::runtime_error("`domain` value is null");
  }
}

void RestServer::GetStatus(const Rest::Request& request,
                           Http::ResponseWriter response) {
  response.headers().add<Http::Header::AccessControlAllowHeaders>(
      "Content-Type");
  response.headers().add<Http::Header::AccessControlAllowMethods>(
      "GET, POST, DELETE, OPTIONS");
  response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
  response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
  response.send(Pistache::Http::Code::Ok);
}

void RestServer::DoAuth(const Rest::Request& request,
                         Http::ResponseWriter response) {
  PrintCookies(request);
  response.cookies().add(Http::Cookie("lang", "fr-FR"));
  response.send(Http::Code::Ok);
}

void RestServer::Shutdown() {
  _http_endpoint->shutdown(); 
}
