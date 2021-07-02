FROM ubuntu:latest

ENV     TZ=Europe/Moscow
RUN     ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN     apt-get -yqq update && \
        apt-get install -yq --no-install-recommends \
                        make gcc g++ python3-pip \
			git cmake libsdl2-dev libsdl2-net-dev libogg-dev \
			libvorbis-dev libavcodec-dev libavformat-dev libavutil-dev && \
        apt-get autoremove -y && \
        apt-get clean -y

WORKDIR /opt/vangers

RUN     GIT_SSL_NO_VERIFY=1 git clone https://github.com/novnc/websockify.git websockify
RUN     cd websockify && \
        pip3 install setuptools && \
        python3 setup.py install && \
        cd ..

RUN     GIT_SSL_NO_VERIFY=1 git clone --depth 1 https://github.com/stalkerg/clunk.git clunk
RUN     GIT_SSL_NO_VERIFY=1 git clone https://github.com/KranX/Vangers vangers

COPY    ./*.h   vangers/server/
COPY    ./*.cpp vangers/server/


RUN     mkdir clunk-build && \
        cd clunk-build && \
        cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_C_COMPILER=/usr/bin/gcc -DCMAKE_CXX_COMPILER=/usr/bin/g++ ../clunk && \
        make -j4 && \
        make install && \
        cd ..

# Debug build is required for generating core dump, see more:
# https://stackoverflow.com/questions/28335614/how-to-generate-core-file-in-docker-container
RUN     mkdir build && \
        cd build && \
        cmake -DCMAKE_BUILD_TYPE=Debug ../vangers && \
        make -j4 vangers_server && \
        cd ..

RUN     mkdir bin && \
        cd bin && \
        cp ../build/server/vangers_server ./server

COPY    ./docker-cmd.sh ./
RUN     chmod 777 docker-cmd.sh

CMD ["./docker-cmd.sh"]
