// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include "abstract_server.h"

AbstractServer::AbstractServer(int num_port, string &classif_config, int debug_mode){
  _num_port = num_port;
  _debug_mode = debug_mode;

  _classifier_controller = make_shared<ClassifierController>(classif_config);
}
