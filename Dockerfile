# Building the server:
#   x64:   $ docker build -t imagename .
#   arm64: $ docker build --platform linux/arm64 -t imagename .
#   multi-platform (build and push):
#     $ docker buildx create --name st-server-multiarch --use
#     $ docker buildx build --platform linux/amd64,linux/arm64 -t imagename:tag --push .

FROM debian:12 AS builder

ENV DEBIAN_FRONTEND=noninteractive
ENV XMAKE_ROOT=y
SHELL ["/bin/bash", "-c"]

# Get packages and xmake
#
# xmake is PINNED. The upstream line had no version, which now pulls xmake 3.x
# onto a tree declaring set_xmakever("2.8.5") -- the toolchain changes under you
# with no diff. 2.9.8 is what .github/workflows/windows.yml builds green with, so
# the container and CI agree. Unpin only once the tree supports 3.x.

RUN apt-get update && apt-get install -y --no-install-recommends build-essential curl pkg-config git ca-certificates unzip libssl-dev && \
    rm -rf /var/lib/apt/lists/* && \
    curl -fsSL https://xmake.io/shget.text | bash -s v2.9.8

# Copy source and build

WORKDIR /src
COPY . /src

RUN source ~/.xmake/profile && \
    xmake config -y -m release && xmake -y && xmake install -y -o package

# Actual server runtime image; distroless for small footprint

FROM gcr.io/distroless/cc-debian12 AS runtime

WORKDIR /st-server

COPY --from=builder \
    /src/package/lib/libSTServer.so \
    /src/package/bin/crashpad_handler \
    /src/package/bin/SkyrimTogetherServer \
    /st-server/

ENTRYPOINT ["./SkyrimTogetherServer"]
EXPOSE 10578/udp
