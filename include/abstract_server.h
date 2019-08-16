// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __ABSTRACT_SERVER_H
#define __ABSTRACT_SERVER_H

#include <iostream>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "classifier.h"

using namespace std;

class AbstractServer {
public:
    AbstractServer(int num_port, string &classif_config, int debug_mode);
    virtual ~AbstractServer() {}; // TODO destroy _list_classifs as well
    virtual void init(size_t thr = 2) = 0;
    virtual void start() = 0;
    virtual void shutdown() = 0;

    std::vector<std::pair<fasttext::real, std::string>>
    askClassification(std::string &text, 
                        std::string &domain,
                        int count,
                        float threshold);
    std::vector<classifier *> getListClassifs();

protected:
  int _debug_mode;
  int _num_port;
  string _classif_config;
  std::vector<classifier *> _list_classifs;

  void ProcessCongifFile(std::string &classif_config, std::vector<classifier *> &_list_classifs);
};

#endif // __ABSTRACT_SERVER_H