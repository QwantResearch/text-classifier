Katanoisi
=========

A new C++ API for Intention classification at Qwant Research.
The API is based on `fasttext`.

## Installation
```
git clone --recursive https://github.com/QwantResearch/Katanoisi.git 
docker build .
``` 

## Launch the API
```
./katanoisi --model-config <filename> [--port <port>] [--threads <nthreads>] [--debug]

--model-config (-f)      config file in which all models are described (REQUIRED)
--port (-p)              port to use (default 9009)
--threads (-t)           number of threads (default 1)
--debug (-d)             debug mode (default false)
--help (-h)              Show this message
```

## Licencing

Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

Contact:
 - christophe[dot]servan[at]qwantresearch[dot]com
 - n[dot]martin[at]qwantresearch[dot]com
