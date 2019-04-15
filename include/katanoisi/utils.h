// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#ifndef __KATA_UTILS_H
#define __KATA_UTILS_H

#include <string>
#include <vector>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

void Split(const std::string &line, std::vector<std::string> &pieces, const std::string del);

void printCookies(const Http::Request &req);

const std::string currentDateTime();

namespace Generic {
void handleReady(const Rest::Request &, Http::ResponseWriter response);
} // namespace Generic

#endif // KATA_UTILS_H