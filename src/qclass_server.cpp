/*
 * MIT License
 *
 * Copyright (c) 2019 Qwant Research
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 * Christophe Servan <c.servan@qwantresearch.com>
 * Noel Martin <n.martin@qwantresearch.com>
 *
 */

#include "qclass_server.h"
#include "qclassifier.h"

#include "qtokenizer.h"

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

namespace Generic {
void handleReady(const Rest::Request&, Http::ResponseWriter response)
{
    response.send(Http::Code::Ok, "1");
}
} // namespace Generic

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

qclass_server::qclass_server(Address addr,
    std::string& classif_config,
    int debug)
{
    httpEndpoint = std::make_shared<Http::Endpoint>(addr);
    _debug_mode = debug;

    std::ifstream model_config;
    model_config.open(classif_config);
    std::string line;

    while (getline(model_config, line)) {
        // @Christophe: this excludes every inline comment
        // WIP
        if (*line.begin() != '#') {
            std::vector<std::string> vecline;
            Split(line, vecline, "\t");
            string domain = vecline[0];
            string file = vecline[1];
            int online = 0;
            if ((int)vecline.size() > 2) {
                online = atoi(vecline[2].c_str());
            }
            cerr << domain << "\t" << file << "\t" << online << endl;
            _list_classifs.push_back(new qclassifier(file, domain));
        }
    }
    model_config.close();
}

void qclass_server::init(size_t thr = 2)
{
    auto opts = Http::Endpoint::options().threads(thr).flags(
        Tcp::Options::InstallSignalHandler);
    httpEndpoint->init(opts);
    setupRoutes();
}

void qclass_server::start()
{
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
    httpEndpoint->shutdown();
}

//   void qclass_server::shutdown() { httpEndpoint->shutdown(); }

//     void setLanguage(string lang)
//     {
//         if (tokenizers.find(lang) != tokenizers.end())
//         {
//             qtokenizer* l_qutok= new qtokenizer(lang);
//
//             pair<string,qtokenizer*> l_pair(lang,l_qutok);
//             tokenizers.insert(l_pair);
//         }
//     }

void qclass_server::setupRoutes()
{
    using namespace Rest;
    Routes::Post(router,
        "/intention/",
        Routes::bind(&qclass_server::doClassificationPost, this));
    Routes::Get(router,
        "/intention/",
        Routes::bind(&qclass_server::doClassificationGet, this));
}

void qclass_server::doClassificationGet(const Rest::Request& request,
    Http::ResponseWriter response)
{
    response.headers().add<Http::Header::AccessControlAllowHeaders>(
        "Content-Type");
    response.headers().add<Http::Header::AccessControlAllowMethods>(
        "GET, POST, DELETE, OPTIONS");
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    string response_string = "{\"domains\":[";
    for (int inc = 0; inc < (int)_list_classifs.size(); inc++) {
        //             cerr << _list_classifs.at(inc)->getDomain() << endl;
        if (inc > 0)
            response_string.append(",");
        response_string.append("\"");
        response_string.append(_list_classifs.at(inc)->getDomain());
        response_string.append("\"");
    }
    response_string.append("]}");
    if (_debug_mode != 0)
        cerr << "LOG: " << currentDateTime() << "\t" << response_string << endl;
    response.send(Pistache::Http::Code::Ok, response_string);
    //         response.send(Pistache::Http::Code::Ok,
    //         "{\"message\":\"success\"}");
}
void qclass_server::doClassificationDomainsGet(const Rest::Request& request,
    Http::ResponseWriter response)
{
    response.headers().add<Http::Header::AccessControlAllowHeaders>(
        "Content-Type");
    response.headers().add<Http::Header::AccessControlAllowMethods>(
        "GET, POST, DELETE, OPTIONS");
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    string response_string = "{\"domains\":[";
    for (int inc = 0; inc < (int)_list_classifs.size(); inc++) {
        //             cerr << _list_classifs.at(inc)->getDomain() << endl;
        if (inc > 0)
            response_string.append(",");
        response_string.append("\"");
        response_string.append(_list_classifs.at(inc)->getDomain());
        response_string.append("\"");
    }
    response_string.append("]}");
    if (_debug_mode != 0)
        cerr << "LOG: " << currentDateTime() << "\t" << response_string << endl;
    response.send(Pistache::Http::Code::Ok, response_string);
}
void qclass_server::doClassificationPost(const Rest::Request& request,
    Http::ResponseWriter response)
{
    response.headers().add<Http::Header::AccessControlAllowHeaders>(
        "Content-Type");
    response.headers().add<Http::Header::AccessControlAllowMethods>(
        "GET, POST, DELETE, OPTIONS");
    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
    nlohmann::json j = nlohmann::json::parse(request.body());
    int count = 10;
    bool debugmode = false;
    if (j.find("count") != j.end()) {
        count = j["count"];
    }
    if (j.find("debug") != j.end()) {
        debugmode = j["debug"];
    }
    if (j.find("language") == j.end()) {
        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        response.send(Http::Code::Bad_Request,
            std::string("the language value is null"));
    }
    if (j.find("text") != j.end()) {
        string text = j["text"];
        string lang = j["language"];
        if (text.length() > 0) {
            qtokenizer l_tok(lang, true);
            j.push_back(nlohmann::json::object_t::value_type(
                string("tokenized"), l_tok.tokenize_str(text)));
            if (_debug_mode != 0)
                cerr << "LOG: " << currentDateTime() << "\t"
                     << "ASK CLASS :\t" << j << endl;
            if (j.find("domain") != j.end()) {
                string domain = j["domain"];
                string tokenized = j["tokenized"];
                std::vector<std::pair<fasttext::real, std::string>> results;
                results = askClassification(tokenized, domain, count);
                j.push_back(
                    nlohmann::json::object_t::value_type(string("intention"), results));
            } else {
                response.headers().add<Http::Header::ContentType>(
                    MIME(Application, Json));
                response.send(Http::Code::Bad_Request,
                    std::string("Domain value is null"));
            }
            std::string s = j.dump();
            if (_debug_mode != 0)
                cerr << "LOG: " << currentDateTime() << "\t" << s << endl;
            response.headers().add<Http::Header::ContentType>(
                MIME(Application, Json));
            response.send(Http::Code::Ok, std::string(s));
        } else {
            response.headers().add<Http::Header::ContentType>(
                MIME(Application, Json));
            response.send(Http::Code::Bad_Request,
                std::string("Length of text value is null"));
        }
    } else {
        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        response.send(Http::Code::Bad_Request,
            std::string("the text value is null"));
    }
}

std::vector<std::pair<fasttext::real, std::string>>
qclass_server::askClassification(std::string& text,
    std::string& domain,
    int count)
{
    std::vector<std::pair<fasttext::real, std::string>> to_return;
    if ((int)text.size() > 0) {
        auto it_classif = std::find_if(
            _list_classifs.begin(),
            _list_classifs.end(),
            [&](qclassifier* l_classif) { return l_classif->getDomain() == domain; });
        if (it_classif != _list_classifs.end()) {
            to_return = (*it_classif)->prediction(text, count);
        }
    }
    return to_return;
}
bool qclass_server::process_localization(string& input, json& output)
{
    string token(input.c_str());
    if (input.find("à ") == 0)
        token = input.substr(3);
    if (input.find("au dessus de ") == 0)
        token = input.substr(13);
    if (input.find("au ") == 0)
        token = input.substr(4);
    if (input.find("vers ") == 0)
        token = input.substr(5);
    output.push_back(
        nlohmann::json::object_t::value_type(string("label"), token));
}

//   void qclass_server::writeLog(string text_to_log) {}

void qclass_server::doAuth(const Rest::Request& request,
    Http::ResponseWriter response)
{
    printCookies(request);
    response.cookies().add(Http::Cookie("lang", "fr-FR"));
    response.send(Http::Code::Ok);
}

// };
