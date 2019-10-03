// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __ABSTRACT_SERVER_H
#define __ABSTRACT_SERVER_H

#include <iostream>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "classifier_controller.h"

using namespace std;

class AbstractServer {
public:
    AbstractServer(int num_port, string& classif_config, int debug_mode);
    virtual ~AbstractServer() {}
    virtual void Init(size_t thr = 2) = 0;
    virtual void Start() = 0;
    virtual void Shutdown() = 0;

protected:
  int _debug_mode;
  int _num_port;
  std::shared_ptr<ClassifierController> _classifier_controller;
};

#endif // __ABSTRACT_SERVER_H