// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include <getopt.h>
#include <iostream>
#include <string>
#include <cstdlib>

#include "rest_server.h"
#include "grpc_server.h"

// default values
int num_port = 9009;
int threads = 1;
int debug = 0;
std::string model_config("");
int server_type = 0; // 0 -> REST, 1 -> GRPC

void usage() {
  cout << "./text-classifier --model-config <filename> [--port <port>] "
          "[--threads <nthreads>] [--debug]\n\n"
          "\t--grpc (-g)              use grpc service instead of rest\n"
          "\t--port (-p)              port to use (default 9009)\n"
          "\t--threads (-t)           number of threads (default 1)\n"
          "\t--model-config (-f)      config file in which all models are "
          "described (needed)\n"
          "\t--debug (-d)             debug mode (default false)\n"
          "\t--help (-h)              Show this message\n"
       << endl;
  exit(1);
}

void ProcessArgs(int argc, char **argv) {
  const char *const short_opts = "p:t:f:dhg";
  const option long_opts[] = {
      {"port", 1, nullptr, 'p'},         {"threads", 1, nullptr, 't'},
      {"model-config", 1, nullptr, 'f'}, {"debug", 0, nullptr, 'd'},
      {"help", 0, nullptr, 'h'},         {"grpc", 0, nullptr, 'g'},
      {nullptr, 0, nullptr, 0}};

  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    if (-1 == opt)
      break;

    switch (opt) {
    case 'p':
      num_port = atoi(optarg);
      break;

    case 't':
      threads = atoi(optarg);
      break;

    case 'd':
      debug = 1;
      break;

    case 'f':
      model_config = optarg;
      break;

    case 'g':
      server_type = 1;
      break;

    case 'h': // -h or --help
    case '?': // Unrecognized option
    default:
      usage();
      break;
    }
  }
  if (model_config == "") {
    cerr << "[ERROR]\t" << currentDateTime() << "\tError, you must set a config file" << endl;
    usage();
    exit(1);
  }
}

int main(int argc, char **argv) {
  if (getenv("API_TC_THREADS") != NULL) { threads=atoi(getenv("API_TC_THREADS")); }
  if (getenv("API_TC_PORT") != NULL) { num_port=atoi(getenv("API_TC_PORT")); }
  if (getenv("API_TC_GRPC") != NULL) { server_type = atoi(getenv("API_TC_GRPC")); }
  if (getenv("API_TC_DEBUG") != NULL) { server_type = atoi(getenv("API_TC_DEBUG")); }
  if (getenv("API_TC_CONFIG") != NULL) { model_config=getenv("API_TC_CONFIG"); }

  ProcessArgs(argc, argv);

  cout << "[INFO]\t" << currentDateTime() << "\tCores = " << hardware_concurrency() << endl;
  cout << "[INFO]\t" << currentDateTime() << "\tUsing " << threads << " threads" << endl;
  cout << "[INFO]\t" << currentDateTime() << "\tUsing port " << num_port << endl;
  cout << "[INFO]\t" << currentDateTime() << "\tUsing config file " << model_config << endl;

  unique_ptr<AbstractServer> classification_api;

  if (server_type == 0) {
    cout << "[INFO]\t" << currentDateTime() << "\tUsing REST API" << endl;
    classification_api = std::unique_ptr<rest_server>(new rest_server(num_port, model_config, debug));
  } else {
    cout << "[INFO]\t" << currentDateTime() << "\tUsing gRPC API" << endl;
    classification_api = std::unique_ptr<grpc_server>(new grpc_server(num_port, model_config, debug));
  }
  classification_api->init(threads); //TODO: Use threads number
  classification_api->start();
  classification_api->shutdown();
}
