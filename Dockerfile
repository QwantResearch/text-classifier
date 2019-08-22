# Copyright 2019 Qwant Research. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

FROM ubuntu:18.04

LABEL authors="Estelle Maudet, Pierre Jackman, Noël Martin, Christophe Servan"

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && \
    apt-get -y install \
        cmake \
        g++ \
        libboost-locale1.65-dev \
        libboost-regex1.65-dev \
        libyaml-cpp-dev \
        git \
        cmake \
        build-essential \
        autoconf \
        libtool \
        pkg-config\
        clang \
        libc++-dev \
        golang \
        libssl-dev \
        libgflags-dev \
        libgtest-dev

# To Keep or not libprotobuf9v5 ??? https://github.com/grpc/grpc/issues/18973
# Change by libprotobuf10 ?
# what about libprotoc9v5 libprotoc9v5

COPY . /opt/text-classifier

WORKDIR /opt/text-classifier

RUN bash ./install.sh

RUN groupadd -r qnlp && useradd --system -s /bin/bash -g qnlp qnlp

USER qnlp 

ENTRYPOINT ["/usr/local/bin/text-classifier"]
