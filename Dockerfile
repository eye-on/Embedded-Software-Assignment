FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# 工具链版本由发行版锁定，保证不同机器上编译结果一致
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        gcc-arm-none-eabi \
        libnewlib-arm-none-eabi \
        binutils-arm-none-eabi \
        make \
        git \
        openocd \
        stlink-tools \
        dfu-util \
        clangd \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
