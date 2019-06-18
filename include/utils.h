// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
// license. See LICENSE in the project root.

#ifndef __UTILS_H
#define __UTILS_H

#include <string>
#include <vector>

#include <pistache/client.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

void Split(const std::string &line, std::vector<std::string> &pieces,
           const std::string del);

void printCookies(const Pistache::Http::Request &req);

const std::string currentDateTime();

namespace Generic {
void handleReady(const Pistache::Rest::Request &req, Pistache::Http::ResponseWriter response);
} // namespace Generic

#endif // __UTILS_H
