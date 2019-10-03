// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "classifier_controller.h"

ClassifierController::ClassifierController(std::string& classif_config) {
    ProcessConfigFile(classif_config, _list_classifs);
}

void ClassifierController::ProcessConfigFile(std::string& classif_config, 
                                             std::vector<Classifier *>& list_classifs) {
    
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

  for (const auto& modelnode : config){
    std::string domain = modelnode.first.as<std::string>();
    YAML::Node modelinfos = modelnode.second;
    std::string filename = modelinfos["filename"].as<std::string>();
    std::string lang = modelinfos["language"].as<std::string>();

    if(domain.empty() || filename.empty() || lang.empty()) {
      std::cerr << "[ERROR] Malformed config for ("
        << domain << ", " << filename << ", " << lang << ")" << std::endl;
      std::cerr << "        Skipped line..." << std::endl;
      continue;
    }

    cout << "[INFO]\t" << domain << "\t" << filename << "\t" << lang ;

    try {
      Classifier* classifier_pointer = new Classifier(filename, domain, lang);
      _list_classifs.push_back(classifier_pointer);
      cout << "\t===> loaded" << endl;
    } catch (std::invalid_argument& inarg) {
      std::cerr << "[ERROR] " << inarg.what() << std::endl;
      continue;
    }
  }
}

std::vector<Classifier *> ClassifierController::GetListClassifs() {
    return _list_classifs;
}

std::vector<std::pair<fasttext::real, std::string>>
ClassifierController::AskClassification(std::string& text, std::string& tokenized, std::string& domain,
                               int count, float threshold) {
  std::vector<std::pair<fasttext::real, std::string>> to_return;
  if ((int)text.size() > 0) {
      auto it_classif = std::find_if(_list_classifs.begin(), _list_classifs.end(),
      [&](Classifier* l_classif) {
                                        return l_classif->GetDomain() == domain;
                                    });
      if (it_classif != _list_classifs.end()) {
          to_return = (*it_classif)->Predict(text, tokenized, count, threshold);
      } else {
          to_return.push_back(std::pair<fasttext::real, std::string>(0.0,"DOMAIN ERROR"));
          // TODO: Deal with DOMAIN ERROR in GRPC
      }
  }
  return to_return;
}

ClassifierController::~ClassifierController() {
  for (auto classifier: _list_classifs){
    delete classifier;
  }
}