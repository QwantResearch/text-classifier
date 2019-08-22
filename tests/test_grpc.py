# coding: utf-8
# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
# license. See LICENSE in the project root.

# To test the grpc server, you need to launch it in another thread.
# Pay attention to use the same port number.
# In case of modification of the proto file, you need to regenerate
# the sources (using generate_python_files_from_proto.sh).

import grpc

import grpc_classification_pb2
import grpc_classification_pb2_grpc

channel = grpc.insecure_channel('localhost:9009') 
stub = grpc_classification_pb2_grpc.RouteClassifyStub(channel)

print("*** GetDomains ***")
domains = stub.GetDomains(grpc_classification_pb2.Empty())
print(domains)

text_to_classify = grpc_classification_pb2.TextToClassify(
    text="Je suis de nationalité Française, et je parle Anglais.",
    count=2,
    threshold=0,
    domain="language_identification")

def generate_to_classify(): 
    to_classify = [text_to_classify]
    to_classify.append(grpc_classification_pb2.TextToClassify(
        text="Have you heard about the search engine Qwant?",
        count=1,
        threshold=0,
        domain="language_identification"))
        
    for elem in to_classify: 
        yield elem    

print("*** GetClassif ***")
classified = stub.GetClassif(text_to_classify)
print(classified)

print("*** StreamClassify ***")
responses = stub.StreamClassify(generate_to_classify())
for response in responses:
    print(response)

        
