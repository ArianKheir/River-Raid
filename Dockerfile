# River Raid Enhanced Edition Dockerfile
# Simplified version for easier debugging

FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV RAYLIB_VERSION=4.5.0

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    git \
    cmake \
    libasound2-dev \
    mesa-common-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    xorg-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Install Raylib from source
WORKDIR /tmp
RUN git clone --branch ${RAYLIB_VERSION} https://github.com/raysan5/raylib.git && \
    cd raylib/src && \
    make PLATFORM=PLATFORM_DESKTOP && \
    make install && \
    ldconfig

# Set up working directory
WORKDIR /app

# Copy source code
COPY . .

# Build the game
RUN make clean && make

# Create non-root user for security
RUN useradd -m -u 1000 riverraid && \
    chown -R riverraid:riverraid /app

# Switch to non-root user
USER riverraid

# Default command
CMD ["./RiverRaid"]

# Labels for documentation
LABEL maintainer="River Raid Enhanced Edition"
LABEL description="A modern graphical version of River Raid using Raylib"
LABEL version="2.0.0"