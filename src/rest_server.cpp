// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "rest_server.h"
#include "utils.h"

rest_server::rest_server(std::string &config_file, int &threads, int debug) 
{
    std::string line;
    int port=9009;
    YAML::Node config;

    try 
    {
    // Reading the configuration file for filling the options.
        config = YAML::LoadFile(config_file);
        cout << "[INFO]\tDomain\t\tLocation/filename\t\tlanguage"<< endl;
        threads = config["threads"].as<int>() ;
        port =  config["port"].as<int>() ;
        debug =  config["debug"].as<int>() ;
        YAML::Node modelconfig = config["models"]; 
        for (const auto& modelnode : modelconfig)
        {
            std::string domain=modelnode.first.as<std::string>();
            YAML::Node modelinfos = modelnode.second;
            std::string filename=modelinfos["filename"].as<std::string>();
            std::string lang=modelinfos["language"].as<std::string>();
            try 
            {
                // Creating the set of models for the API
                if ((int) filename.size() == 0)
                {
                    cerr << "[ERROR]\tModel filename is not set for " << domain << endl;
                    continue;
                }
                cout << "[INFO]\t"<< domain << "\t" << filename << "\t" << lang ;
                classifier* classifier_pointer = new classifier(filename, domain, lang);
                _list_classifs.push_back(classifier_pointer);
                cout << "\t===> loaded" << endl;
            } 
            catch (invalid_argument& inarg) 
            {
                cerr << "[ERROR]\t" << inarg.what() << endl;
                continue;
            }
        }
    } catch (YAML::BadFile& bf) {
      cerr << "[ERROR]\t" << bf.what() << endl;
      exit(1);
    }
  
     

    cout << "[INFO]\tnumber of threads:\t"<< threads << endl;
    cout << "[INFO]\tport used:\t"<< port << endl;
    if (debug > 0) cout << "[INFO]\tDebug mode activated" << endl;
    else cout << "[INFO]\tDebug mode desactivated" << endl;
    if ((int)_list_classifs.size() == 0) 
    {
        cerr << "[ERROR]\tNo classification model loaded, exiting." << endl;
        exit(1);
    }
    // Creating the entry point of the REST API.
    Pistache::Port pport(port);
    Address addr(Ipv4::any(), pport);
    httpEndpoint = std::make_shared<Http::Endpoint>(addr);
    _debug_mode = debug;

}


void rest_server::init(size_t thr) {
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

  Routes::Post(router, "/intention/",
               Routes::bind(&rest_server::doClassificationPost, this));

  Routes::Post(router, "/intention_batch/",
              Routes::bind(&rest_server::doClassificationBatchPost, this));

  Routes::Get(router, "/intention/",
              Routes::bind(&rest_server::doClassificationGet, this));
}

void rest_server::doClassificationGet(const Rest::Request &request,
                                      Http::ResponseWriter response) {
  response.headers().add<Http::Header::AccessControlAllowHeaders>(
      "Content-Type");
  response.headers().add<Http::Header::AccessControlAllowMethods>(
      "GET, POST, DELETE, OPTIONS");
  response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
  response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
  string response_string = "{\"domains\":[";
  for (int inc = 0; inc < (int)_list_classifs.size(); inc++) {
    if (inc > 0)
      response_string.append(",");
    response_string.append("\"");
    response_string.append(_list_classifs.at(inc)->getDomain());
    response_string.append("\"");
  }
  response_string.append("]}");
  if (_debug_mode != 0)
    cerr << "[DEBUG]\t" << currentDateTime() << "\tRESPONSE\t" << response_string << endl;
  response.send(Pistache::Http::Code::Ok, response_string);
}

void rest_server::doClassificationPost(const Rest::Request &request,
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
    rest_server::fetchParamWithDefault(j, domain, count, threshold, debugmode);
  } catch (std::runtime_error e) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request, e.what());
  }

  if (j.find("text") != j.end()) 
  {
    string text = j["text"];
    string tokenized;
    if (_debug_mode != 0)
      cerr << "[DEBUG]\t" << currentDateTime() << "\t" << "ASK CLASS :\t" << j << endl;
    std::vector<std::pair<fasttext::real, std::string>> results;
    results = askClassification(text, tokenized, domain, count, threshold);
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

void rest_server::doClassificationBatchPost(const Rest::Request &request,
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
    rest_server::fetchParamWithDefault(j, domain, count, threshold, debugmode);
  } catch (std::runtime_error e) {
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Bad_Request, e.what());
  }
  
  if (j.find("batch_data") != j.end()) {
    for (auto& it: j["batch_data"]){
      if (it.find("text") != it.end()) {
        string text = it["text"];
        string tokenized;
        it.push_back(nlohmann::json::object_t::value_type(string("tokenized"), tokenized));
        if (_debug_mode != 0)
          cerr << "[DEBUG]\t" << currentDateTime() << "\tASK CLASS:\t" << it << endl;
        auto results = askClassification(text, tokenized, domain, count, threshold);
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

void rest_server::fetchParamWithDefault(const nlohmann::json& j, 
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

std::vector<std::pair<fasttext::real, std::string>>
rest_server::askClassification(std::string &text, std::string &tokenized_text, std::string &domain,
                               int count, float threshold) {
  std::vector<std::pair<fasttext::real, std::string>> to_return;
  if ((int)text.size() > 0) 
  {
      auto it_classif = std::find_if(_list_classifs.begin(), _list_classifs.end(), [&](classifier *l_classif)
                                    {
                                        return l_classif->getDomain() == domain;
                                    });
      if (it_classif != _list_classifs.end()) 
      {
          to_return = (*it_classif)->prediction(text, tokenized_text, count, threshold);
      }
      else
      {
          to_return.push_back(std::pair<fasttext::real, std::string>(0.0,"DOMAIN ERROR"));
      }
  }
  return to_return;
}

void rest_server::doAuth(const Rest::Request &request,
                         Http::ResponseWriter response) {
  printCookies(request);
  response.cookies().add(Http::Cookie("lang", "fr-FR"));
  response.send(Http::Code::Ok);
}
