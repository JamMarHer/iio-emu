#!/bin/bash -e

handle_default() {
    sudo apt-get -qq update
    sudo DEBIAN_FRONTEND=noninteractive apt-get install -y cmake libxml2-dev rpm git
}

handle_ubuntu() {
    handle_default
}

handle_${BUILD_TYPE}
