// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "classifier_controller.h"

ClassifierController::ClassifierController(std::string &classif_config) {
    ProcessConfigFile(classif_config, _list_classifs);
}

void ClassifierController::ProcessConfigFile(std::string &classif_config, 
                                             std::vector<classifier *> &_list_classifs) {
    
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

std::vector<classifier *> ClassifierController::getListClassifs() {
    return _list_classifs;
}

std::vector<std::pair<fasttext::real, std::string>>
ClassifierController::askClassification(std::string &text, std::string &domain,
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

ClassifierController::~ClassifierController() {
  for (auto classifier: _list_classifs){
    delete classifier;
  }
}