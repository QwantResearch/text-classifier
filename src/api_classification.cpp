/***************************************************************************
* MIT License
* 
* Copyright (c) 2018 Christophe SERVAN, Qwant Research, 
* email: christophe[dot]servan[at]qwantresearch[dot]com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
***************************************************************************/
#include <algorithm>

#include <iostream>
#include <pistache/http.h>
#include <pistache/client.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <nlohmann/json.hpp>
#include <map>
#include <fasttext/fasttext.h>
#include <sstream>
#include "qtokenizer.h"
#include <time.h>

using namespace std;
using namespace nlohmann;
using namespace fasttext;
using namespace Pistache;

void printCookies(const Http::Request& req) {
    auto cookies = req.cookies();
    std::cout << "Cookies: [" << std::endl;
    const std::string indent(4, ' ');
    for (const auto& c: cookies) {
        std::cout << indent << c.name << " = " << c.value << std::endl;
    }
    std::cout << "]" << std::endl;
}

void Split(const std::string& line, std::vector<std::string>& pieces, const std::string del) {
  size_t begin = 0;
  size_t pos = 0;
  std::string token;
  while ((pos = line.find(del, begin)) != std::string::npos) {
    if (pos > begin) {
      token = line.substr(begin, pos - begin);
      if(token.size() > 0)
        pieces.push_back(token);
    }
    begin = pos + del.size();
  }
  if (pos > begin) {
    token = line.substr(begin, pos - begin);
  }
  if(token.size() > 0)
    pieces.push_back(token);
}

namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "1");
}

}
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

class Classifier {
public:
    Classifier(std::string& filename, string& domain)
    {
        _model.loadModel(filename.c_str());
        _domain = domain;
    }
    
    vector < pair < fasttext::real, string > > prediction(string& text, int count)
    {
        vector <pair <fasttext::real, string> > results;
        if (text.find("\n") != (int)text.size()-1) text.push_back('\n');
        istringstream istr(text);
        _model.predict(istr,count,results);
        for (int i=0;i<(int)results.size();i++)
        {
            results.at(i).first=exp(results.at(i).first);
            results.at(i).second=results.at(i).second.substr(9);
        }
        return results;
    }
    std::string getDomain()
    {
        return _domain;
    }

private:
    std::string _domain;
    fasttext::FastText _model;
};


class StatsEndpoint {
public:
    StatsEndpoint(Address addr, string classif_config, int debug_mode)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { 
        ifstream model_config;
        string line;
        _debug_mode=debug_mode;
        model_config.open(classif_config);
//         Classifier * l_classif;
        while (getline(model_config,line))
        {
            if (line.find("#") != 0)
            {
                vector<string> vecline;
                Split(line,vecline,"\t");
    //             string domain=line.substr(0,line.find("\t"));
    //             string file=line.substr(line.find("\t")+1);
                string domain=vecline[0];
                string file=vecline[1];
                int online=0;
                if ((int)vecline.size()>2)
                {
                  online=atoi(vecline[2].c_str());
                }
                cerr << domain <<"\t"<< file << "\t" << online << endl;
                _list_classifs.push_back(new Classifier(file,domain));
            }
        }
        model_config.close();
        
      
    }

    void init(size_t thr = 2) 
    {
        auto opts = Http::Endpoint::options()
            .threads(thr)
            .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() 
    {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() 
    {
        httpEndpoint->shutdown();
    }
    
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

private:
    vector<Classifier*> _list_classifs;
    FastText clang;
    FastText cIoT;
    FastText cint;
    FastText cshop;
    int _debug_mode;
//    cpr::Response *r;
    
    void setupRoutes() {
        using namespace Rest;
        Routes::Post(router,"/intention/", Routes::bind(&StatsEndpoint::doClassificationPost, this));
        Routes::Get(router, "/intention/", Routes::bind(&StatsEndpoint::doClassificationGet, this));

    }
    void doClassificationGet(const Rest::Request& request, Http::ResponseWriter response) {
        response.headers().add<Http::Header::AccessControlAllowHeaders>("Content-Type");
        response.headers().add<Http::Header::AccessControlAllowMethods>("GET, POST, DELETE, OPTIONS");
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        string response_string="{\"domains\":[";
        for (int inc=0; inc<(int)_list_classifs.size(); inc++)
        {
//             cerr << _list_classifs.at(inc)->getDomain() << endl;
            if (inc>0) response_string.append(",");
            response_string.append("\"");
            response_string.append(_list_classifs.at(inc)->getDomain());
            response_string.append("\"");
        }
        response_string.append("]}");
        if (_debug_mode != 0 ) cerr << "LOG: "<< currentDateTime() << "\t" << response_string << endl;
        response.send(Pistache::Http::Code::Ok, response_string);
//         response.send(Pistache::Http::Code::Ok, "{\"message\":\"success\"}");
    }
    void doClassificationDomainsGet(const Rest::Request& request, Http::ResponseWriter response) {
        response.headers().add<Http::Header::AccessControlAllowHeaders>("Content-Type");
        response.headers().add<Http::Header::AccessControlAllowMethods>("GET, POST, DELETE, OPTIONS");
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        string response_string="{\"domains\":[";
        for (int inc=0; inc<(int)_list_classifs.size(); inc++)
        {
//             cerr << _list_classifs.at(inc)->getDomain() << endl;
            if (inc>0) response_string.append(",");
            response_string.append("\"");
            response_string.append(_list_classifs.at(inc)->getDomain());
            response_string.append("\"");
        }
        response_string.append("]}");
        if (_debug_mode != 0 ) cerr << "LOG: "<< currentDateTime() << "\t" << response_string << endl;
        response.send(Pistache::Http::Code::Ok, response_string);
    }
    void doClassificationPost(const Rest::Request& request, Http::ResponseWriter response) 
    {
        response.headers().add<Http::Header::AccessControlAllowHeaders>("Content-Type");
        response.headers().add<Http::Header::AccessControlAllowMethods>("GET, POST, DELETE, OPTIONS");
        response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
        nlohmann::json j = nlohmann::json::parse(request.body());
        int count=10;
        bool debugmode = false;
        if (j.find("count") != j.end())
        {
            count=j["count"]; 
        }
        if (j.find("debug") != j.end())
        {
            debugmode=j["debug"]; 
        }
        if (j.find("language") == j.end())
        {
            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Bad_Request, std::string("the language value is null"));
        }
        if (j.find("text") != j.end())
        {
            string text=j["text"]; 
            string lang=j["language"]; 
            if (text.length() > 0)
            {
                qtokenizer l_tok(lang,true);
                j.push_back( nlohmann::json::object_t::value_type(string("tokenized"), l_tok.tokenize_str(text) ));
                if (_debug_mode != 0 ) cerr << "LOG: "<< currentDateTime() << "\t" << "ASK CLASS :\t" << j << endl;
                if (j.find("domain") != j.end())
                {
                    string domain=j["domain"]; 
                    string tokenized=j["tokenized"]; 
                    std::vector < std::pair < fasttext::real, std::string > > results;
                    results = askClassification(tokenized,domain,count);
                    j.push_back( nlohmann::json::object_t::value_type(string("intention"), results));
                }
                else
                {
                    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                    response.send(Http::Code::Bad_Request, std::string("Domain value is null"));
                }
                std::string s=j.dump();
                if (_debug_mode != 0 ) cerr << "LOG: "<< currentDateTime() << "\t" << s << endl;
                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Ok, std::string(s));
            }
            else
            {
                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Bad_Request, std::string("Length of text value is null"));
            }
        }
        else
        {
            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Bad_Request, std::string("the text value is null"));
        }
    }    
    
    std::vector < std::pair < fasttext::real, std::string > > askClassification(std::string& text, std::string& domain, int count)
    {
        std::vector < std::pair < fasttext::real, std::string > > to_return;
	if ((int)text.size() > 0)
	{
	        auto it_classif = std::find_if(_list_classifs.begin(), _list_classifs.end(), [&](Classifier* l_classif) 
	        {
	            return l_classif->getDomain() == domain;
	        }); 
	        if (it_classif != _list_classifs.end())
	        {
	            to_return = (*it_classif)->prediction(text,count);
	        }
	}
        return to_return;
    }
    bool process_localization(string& input, json &output)
    {
        string token(input.c_str());
        if (input.find("à ") == 0) token=input.substr(3);
        if (input.find("au dessus de ") == 0) token=input.substr(13);
        if (input.find("au ") == 0) token=input.substr(4);
        if (input.find("vers ") == 0) token=input.substr(5);
        output.push_back(nlohmann::json::object_t::value_type(string("label"), token ));
    }
    
    void writeLog(string text_to_log)
    {
        
    }


    void doAuth(const Rest::Request& request, Http::ResponseWriter response) {
        printCookies(request);
        response.cookies()
            .add(Http::Cookie("lang", "fr-FR"));
        response.send(Http::Code::Ok);
    }

    typedef std::mutex Lock;
    typedef std::lock_guard<Lock> Guard;
    Lock nluLock;
//     std::vector<Classifier> nluDomains;
    

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main(int argc, char *argv[]) {
    Port port(9009);

    int thr = 8;
    int debug_mode = 0;
    string model_config_classif("model_classif_config.txt");
    if (argc >= 2) 
    {
        port = std::stol(argv[1]);
        if (argc >= 3)
        {
            thr = std::stol(argv[2]);
            if (argc >= 4)
            {
                model_config_classif = string(argv[3]);
                if (argc >= 5)
                {
                        debug_mode = atoi(argv[4]);
                }
            }
        }
    }

    Address addr(Ipv4::any(), port);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << thr << " threads" << endl;
    

    StatsEndpoint stats(addr,model_config_classif,debug_mode);

    stats.init(thr);
    stats.start();
    stats.shutdown();
}
