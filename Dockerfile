FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    libasio-dev \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

RUN curl -fL \
    https://github.com/CrowCpp/Crow/releases/download/v1.0+5/crow_all.h \
    -o crow_all.h && ls -lh crow_all.h

COPY main.cpp .

RUN g++ --version

RUN g++ -std=c++17 -O2 \
    -DCROW_USE_BOOST=0 \
    -DASIO_STANDALONE \
    -I/usr/include \
    -o vastu_api main.cpp \
    -lpthread -lssl -lcrypto -lz

RUN strip vastu_api && echo "=== Build OK ==="

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    zlib1g libssl3 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/vastu_api .

EXPOSE 8080
CMD ["./vastu_api"]
