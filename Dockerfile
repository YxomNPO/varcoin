FROM ubuntu
MAINTAINER Pavel Doomkin

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update; \
    apt install build-essential wget git cmake cmake-data qt5-default libboost1.58-all-dev -y; \
    git clone https://gitee.com/yxom-fylzz/varcoin && cd varcoin; \
    cmake . && make; \
    cd ../ && git clone https://gitee.com/yxom-fylzz/varcoin-wallet-gui && cd varcoin-wallet-gui && cp ../varcoin/* varnote -r; \
    cmake . && make