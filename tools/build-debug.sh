#!/bin/bash

HERE=$( cd $(dirname $0) && pwd )

BUILD_DIR=${HERE}/../build.debug
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DCMAKE_BUILD_TYPE=Debug ..
make
