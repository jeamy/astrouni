FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        ca-certificates \
        software-properties-common \
        gnupg \
        build-essential \
        cmake \
        git \
        zip \
        pkg-config \
        patchelf \
        libgl1-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

RUN add-apt-repository -y ppa:okirby/qt6-backports \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        qt6-base-dev \
        qt6-tools-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
