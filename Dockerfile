FROM ubuntu:18.04

LABEL maintainer "KatanoisiDocker"

USER root

ENV LANGUAGE="fr_FR.UTF-8"
ENV LANG="fr_FR.UTF-8"
ENV LC_ALL="fr_FR.UTF-8"

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/

RUN apt-get update -y && \
        apt-get install -y software-properties-common vim bash locales

RUN locale-gen fr_FR.UTF-8

RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
    sed -i -e 's/# fr_FR.UTF-8 UTF-8/fr_FR.UTF-8 UTF-8/' /etc/locale.gen && \
    echo 'LANG="fr_FR.UTF-8"'>/etc/default/locale && \
    dpkg-reconfigure --frontend=noninteractive locales && \
    update-locale LANG=fr_FR.UTF-8

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN echo "$LANGUAGE $LANG $LC_ALL"

RUN apt-get -y install sudo git cmake g++ libboost-locale-dev libboost-regex-dev

RUN useradd -ms /bin/bash qnlp

USER qnlp

ADD . /opt/src

WORKDIR /home/qnlp/
