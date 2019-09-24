# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0
# license. See LICENSE in the project root.

proto_file="../protos/grpc_classification.proto"
proto_dir=`dirname $proto_file`

python3 -m grpc_tools.protoc -I $proto_dir --python_out=. --grpc_python_out=. $proto_file