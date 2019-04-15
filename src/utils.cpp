// Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "katanoisi/utils.h"

void printCookies(const Http::Request& req)
{
    auto cookies = req.cookies();
    const std::string indent(4, ' ');

    std::cout << "Cookies: [" << std::endl;
    for (const auto& c : cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}

void Split(const std::string& line,
    std::vector<std::string>& pieces,
    const std::string del)
{
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

const std::string
currentDateTime()
{
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
void handleReady(const Rest::Request&, Http::ResponseWriter response)
{
    response.send(Http::Code::Ok, "1");
}
} // namespace Generic
