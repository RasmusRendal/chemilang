FROM alpine

RUN apk add clang boost-iostreams boost-dev libstdc++ cmake make build-base gcc flex bison libc-dev gtest gtest-dev bash gcovr

RUN mkdir /chem

WORKDIR /chem

COPY CMakeLists.txt ./
COPY gencov.sh ./
COPY src ./src/
COPY chemlib/ ./
COPY tests/ ./

RUN sh -c "cmake -DCMAKE_BUILD_TYPE=Debug . && make && ./bin/tests"
