#!/bin/bash -e

handle_default() {
    mkdir -p build
    cd build

    cmake ..
    make package
    cd ..
}

handle_ubuntu() {
    handle_default
}

handle_${BUILD_TYPE}
