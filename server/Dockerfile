# Build from the repository root with: docker build -f server/Dockerfile .
FROM docker.io/library/ubuntu:latest

ENV     TZ=Europe/Moscow
RUN     ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN     apt-get -yqq update && \
        apt-get install -yq --no-install-recommends \
                        build-essential ca-certificates curl git ninja-build \
                        python3-venv libogg-dev libvorbis-dev libavcodec-dev \
                        libavformat-dev libavutil-dev libswscale-dev zlib1g-dev && \
        apt-get autoremove -y && \
        apt-get clean -y

RUN     python3 -m venv /opt/vangers-tools && \
        /opt/vangers-tools/bin/python -m pip install --upgrade pip && \
        /opt/vangers-tools/bin/python -m pip install "cmake>=4.0.0" setuptools
ENV     PATH="/opt/vangers-tools/bin:${PATH}"

WORKDIR /opt/vangers

ARG     SDL_VERSION=3.4.12
ARG     SDL_SHA256=f07b958a9ac5020fb7a44cadb957f658b2149c3c8abb4f63145fac9303249db7
ARG     SDL_NET_VERSION=3.2.0
ARG     SDL_NET_SHA256=098522fc26d4e302ef9348aee6e76e67fe504dfefd7f596236568f8330570c41
ARG     CLUNK_COMMIT=d3fabbf95067f5350e16128b51dc17505e0f83bd

RUN     curl --fail --location --retry 3 \
            --output SDL3.tar.gz \
            "https://github.com/libsdl-org/SDL/releases/download/release-${SDL_VERSION}/SDL3-${SDL_VERSION}.tar.gz" && \
        echo "${SDL_SHA256}  SDL3.tar.gz" | sha256sum --check --strict && \
        tar --extract --gzip --file SDL3.tar.gz && \
        cmake -S "SDL3-${SDL_VERSION}" -B SDL3-build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local \
            -DSDL_SHARED=ON \
            -DSDL_STATIC=OFF \
            -DSDL_UNIX_CONSOLE_BUILD=ON \
            -DSDL_TESTS=OFF \
            -DSDL_EXAMPLES=OFF && \
        cmake --build SDL3-build --parallel && \
        cmake --install SDL3-build && \
        rm -rf SDL3.tar.gz "SDL3-${SDL_VERSION}" SDL3-build

RUN     curl --fail --location --retry 3 \
            --output SDL3_net.tar.gz \
            "https://github.com/libsdl-org/SDL_net/releases/download/release-${SDL_NET_VERSION}/SDL3_net-${SDL_NET_VERSION}.tar.gz" && \
        echo "${SDL_NET_SHA256}  SDL3_net.tar.gz" | sha256sum --check --strict && \
        tar --extract --gzip --file SDL3_net.tar.gz && \
        cmake -S "SDL3_net-${SDL_NET_VERSION}" -B SDL3_net-build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local \
            -DBUILD_SHARED_LIBS=ON \
            -DSDLNET_SAMPLES=OFF && \
        cmake --build SDL3_net-build --parallel && \
        cmake --install SDL3_net-build && \
        rm -rf SDL3_net.tar.gz "SDL3_net-${SDL_NET_VERSION}" SDL3_net-build

RUN     git clone --depth 1 https://github.com/novnc/websockify.git websockify && \
        python -m pip install ./websockify

RUN     git init clunk && \
        git -C clunk remote add origin https://github.com/stalkerg/clunk.git && \
        git -C clunk fetch --depth 1 origin "${CLUNK_COMMIT}" && \
        git -C clunk checkout --detach FETCH_HEAD

RUN     cmake -S clunk -B clunk-build -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local && \
        cmake --build clunk-build --parallel && \
        cmake --install clunk-build && \
        ldconfig

COPY    CMakeLists.txt vangers.res vangers/
COPY    cmake vangers/cmake
COPY    lib vangers/lib
COPY    server vangers/server
COPY    src vangers/src
COPY    surmap vangers/surmap

# Debug build is required for generating core dump, see more:
# https://stackoverflow.com/questions/28335614/how-to-generate-core-file-in-docker-container
RUN     cmake -S vangers -B build -G Ninja \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=OFF && \
        cmake --build build --target vangers_server --parallel

RUN     mkdir bin && cp build/server/vangers_server bin/server

COPY    server/docker-cmd.sh ./
RUN     chmod 777 docker-cmd.sh

CMD ["./docker-cmd.sh"]
