// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#include <getopt.h>
#include <iostream>
#include <string>

#include "rest_server.h"

// default number of threads
int threads = 1;
// debug mode set to zero by default
int debug = 0;
std::string API_config("");

void usage() {
  cout << "./text-classifier --config <filename> [--debug]\n\n"
          "\t--config (-c)            config file in which all models and API configuration are set (needed)\n"
          "\t--debug (-d)             debug mode (default false)\n"
          "\t--help (-h)              Show this message\n"
       << endl;
  exit(1);
}

void ProcessArgs(int argc, char **argv) {
  const char *const short_opts = "t:c:dh";
  const option long_opts[] = {
      {"threads", 0, nullptr, 't'}, 
      {"config", 1, nullptr, 'f'}, 
      {"debug", 0, nullptr, 'd'},
      {"help", 0, nullptr, 'h'},
      {nullptr, 0, nullptr, 0}};

  while (true) {
    const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

    if (-1 == opt)
      break;

    switch (opt) {
    case 'd':
      debug = 1;
      break;

    case 'c':
      API_config = optarg;
      break;

    case 't':
      threads = atoi(optarg);
      break;

    case 'h': // -h or --help
    case '?': // Unrecognized option
    default:
      usage();
      break;
    }
  }
  if (API_config == "") {
    cerr << "Error, you must set a config file" << endl;
    usage();
    exit(1);
  }
}

int main(int argc, char **argv) {
  ProcessArgs(argc, argv);
  cout << "[INFO]\tUsing config file:\t" << API_config << endl;
  cout << "[INFO]\tCores available:\t" << hardware_concurrency() << endl;
  // creating the rest server, the options in the config file overite the ones given in the arguments.
  rest_server classification_api(API_config, threads, debug);

  classification_api.init();
  classification_api.start();
  classification_api.shutdown();
}
