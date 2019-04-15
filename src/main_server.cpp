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
#include <getopt.h>

int num_port = 9009;
int threads = 1;
int debug = 0;
std::string model_config("");

void usage()
{
    cout << "./qclass_server --model-config <filename> [--port <port>] "
            "[--threads <nthreads>] [--debug]\n\n"
            "\t--port (-p)              port to use (default 9009)\n"
            "\t--threads (-t)           number of threads (default 1)\n"
            "\t--model-config (-f)      config file in which all models are "
            "described (needed)\n"
            "\t--debug (-d)             debug mode (default false)\n"
            "\t--help (-h)              Show this message\n"
         << endl;
    exit(1);
}

void ProcessArgs(int argc, char** argv)
{
    const char* const short_opts = "p:t:f:dh";
    const option long_opts[] = {
        { "port", 1, nullptr, 'p' }, { "threads", 1, nullptr, 't' },
        { "model-config", 1, nullptr, 'f' }, { "debug", 0, nullptr, 'd' },
        { "help", 0, nullptr, 'h' }, { nullptr, 0, nullptr, 0 }
    };

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

        case 'h': // -h or --help
        case '?': // Unrecognized option
        default:
            usage();
            break;
        }
    }
    if (model_config == "") {
        cerr << "Error, you must set a config file" << endl;
        usage();
        exit(1);
    }
}

int main(int argc, char** argv)
{
    ProcessArgs(argc, argv);
    Pistache::Port port(num_port);

    Address addr(Ipv4::any(), port);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << threads << " threads" << endl;
    cout << "Using port " << port << endl;
    cout << "Using config file " << model_config << endl;

    qclass_server classification_api(addr, model_config, debug);

    classification_api.init(threads);
    classification_api.start();
    classification_api.shutdown();
}
