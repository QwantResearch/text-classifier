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


#include "qclass_api.h"
#include "qclassifier.h"
#include "qtokenizer.h"


int main(int argc, char **argv) {
  Pistache::Port port(9009);
  int threads(8);
  std::string model_config("");
  int debug(0);

  for (int i = 1; i < argc; i += 2) 
  {
    if (strcmp(argv[i], "--port") == 0) {
        port = std::atoi(argv[i+1]);
    } else if (strcmp(argv[i], "--threads") == 0) {
        threads = std::atoi(argv[i+1]);
    } else if (strcmp(argv[i], "--model-path") == 0) {
        model_config = std::string(argv[i+1]);
    } else if (strcmp(argv[i], "--debug") == 0) {
      debug = 1;
    } else if (strcmp(argv[i], "--help")) {
      std::cout << "./qclass_server --model-config <filename> [--port <port>] [--threads <nthreads>] [--debug]" << std::endl;
      return 0;
    } else {
      std::cerr << "Unkown option: " << argv[i] << std::endl;
      std::cerr << "Exiting..." << std::endl;
      return 1;
    }
  }


    Address addr(Ipv4::any(), port);

    cout << "Cores = " << hardware_concurrency() << endl;
    cout << "Using " << threads << " threads" << endl;
    

    qclass_api classification_api(addr,model_config,debug);

    classification_api.init(threads);
    classification_api.start();
    classification_api.shutdown();
}
