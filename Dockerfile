# ─── Stage 1: Build ───────────────────────────────────────────────────────────
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install build tools + Crow dependencies (Boost, asio, zlib)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    curl \
    libboost-all-dev \
    libssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Download Crow single-header (header-only, no separate build needed)
RUN curl -fL \
    https://github.com/CrowCpp/Crow/releases/download/v1.0+5/crow_all.h \
    -o crow_all.h

# Copy source and compile
COPY main.cpp .

RUN g++ -std=c++17 -O2 -o vastu_api main.cpp \
    -lboost_system -lpthread -lz \
    && strip vastu_api

# ─── Stage 2: Runtime ─────────────────────────────────────────────────────────
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    libboost-thread1.74.0 \
    zlib1g \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/vastu_api .

# Render injects PORT at runtime; 8080 is the default
EXPOSE 8080

CMD ["./vastu_api"]
