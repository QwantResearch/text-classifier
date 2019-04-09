FROM ubuntu:18.04

LABEL maintainer "KatanoisiDocker"

USER root

ENV LANGUAGE="fr_FR.UTF-8"
ENV LANG="fr_FR.UTF-8"
ENV LC_ALL="fr_FR.UTF-8"

ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/

# ENV http_proxy=http://10.100.9.1:2001
# ENV https_proxy=http://10.100.9.1:2001

# RUN echo "export http_proxy=http://10.100.9.1:2001" >> /etc/profile
# RUN echo "export https_proxy=http://10.100.9.1:2001" >> /etc/profile
# RUN echo "Acquire::http::Proxy \"http://10.100.9.1:2001\";" >> /etc/apt/apt.conf

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

run mkdir /DEV/Tools/Sources/ -pv
WORKDIR /DEV

run git clone https://github.com/facebookresearch/fastText.git Tools/Sources/fastText && cd Tools/Sources/fastText && mkdir -pv build && cd build && cmake .. && make -j4 && make install     
run git clone https://github.com/QwantResearch/qnlp-toolkit.git Tools/Sources/qnlp-toolkit && cd Tools/Sources/qnlp-toolkit && mkdir -pv build && cd build && cmake .. && make -j4 && make install
run git clone https://github.com/QwantResearch/pistache.git Tools/Sources/pistache && cd Tools/Sources/pistache && mkdir -pv build && cd build && cmake .. && make -j4 && make install
run git clone https://github.com/nlohmann/json.git Tools/Sources/json && cd Tools/Sources/json && mkdir -pv build && cd build && cmake .. && make -j4 && make install
run git clone https://github.com/QwantResearch/qclassification.git Tools/Sources/qclassification && cd Tools/Sources/qclassification && git checkout cpp && mkdir -pv build && cd build && cmake .. && make -j4 && make install

USER qnlp

WORKDIR /home/qnlp/

