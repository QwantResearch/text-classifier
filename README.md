Text Classifier
=========

A new C++ API for Intention classification at Qwant Research.
The API is based on [`fasttext`](https://fasttext.cc/).

## Installation
```
git clone --recursive https://github.com/QwantResearch/text-classifier.git 
cd text-classifier
docker build -t text-classifier:latest .
``` 

## Launch the API
```
./text-classifier --model-config <filename> [--port <port>] [--threads <nthreads>] [--debug]

--model-config (-f)      config file in which all models are described (REQUIRED)
--port (-p)              port to use (default 9009)
--threads (-t)           number of threads (default 1)
--debug (-d)             debug mode (default false)
--grpc (-g)              use grpc service instead of rest
--help (-h)              show this message
```

You can also use environment variable to configure the API:
 * threads can be set with API_TC_THREADS
 * port can be set with API_TC_PORT
 * model-config can be set with API_TC_CONFIG
 * grpc can be set with API_TC_GRPC
 * debug can be set with API_TC_DEBUG

## Licencing

Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

Contact:
 - e[dot]maudet[at]qwantresearch[dot]com
 - p[dot]jackman[at]qwantresearch[dot]com
 - n[dot]martin[at]qwantresearch[dot]com
 - christophe[dot]servan[at]qwantresearch[dot]com
