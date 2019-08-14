// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "utils.h"

void printCookies(const Pistache::Http::Request &req) {
  auto cookies = req.cookies();
  const std::string indent(4, ' ');

  std::cout << "Cookies: [" << std::endl;
  for (const auto &c : cookies) {
    std::cout << indent << c.name << " = " << c.value << std::endl;
  }
  std::cout << "]" << std::endl;
}

void Split(const std::string &line, std::vector<std::string> &pieces,
           const std::string del) {
  size_t begin = 0;
  size_t pos = 0;
  std::string token;

  while ((pos = line.find(del, begin)) != std::string::npos) {
    if (pos > begin) {
      token = line.substr(begin, pos - begin);
      if (token.size() > 0)
        pieces.push_back(token);
    }
    begin = pos + del.size();
  }
  if (pos > begin) {
    token = line.substr(begin, pos - begin);
  }
  if (token.size() > 0)
    pieces.push_back(token);
}

void ProcessCongifFile(std::string &classif_config, std::vector<classifier *> &_list_classifs){

  std::ifstream model_config;
  model_config.open(classif_config);
  std::string line;

  YAML::Node config;
  try {
    config = YAML::LoadFile(classif_config);
  } catch (YAML::BadFile& bf) {
    std::cerr << "[ERROR] " << bf.what() << std::endl;
    exit(1);
  }

  for (const auto& line : config){
    std::string domain = line.first.as<std::string>();
    std::string file = line.second.as<std::string>();

    if(domain.empty() || file.empty()) {
      std::cerr << "[ERROR] Malformed config for pair ("
        << domain << ", " << file << ")" << std::endl;
      std::cerr << "        Skipped line..." << std::endl;
      continue;
    }

    std::cout << domain << "\t" << file << "\t" << std::endl;

    try {
      classifier* classifier_pointer = new classifier(file, domain);
      _list_classifs.push_back(classifier_pointer);
    } catch (std::invalid_argument& inarg) {
      std::cerr << "[ERROR] " << inarg.what() << std::endl;
      continue;
    }
  }

}

const std::string currentDateTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
  // for more information about date/time format
  strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

  return buf;
}

namespace Generic {
void handleReady(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter response) {
  response.send(Pistache::Http::Code::Ok, "1");
}
} // namespace Generic
