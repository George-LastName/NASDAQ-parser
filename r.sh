#!/usr/bin/env bash

set -e

clean() {
    rm -rf build
}

build() {
    cmake -S . -B build
    cmake --build build
}

run() {
    ./build/main 01302019.NASDAQ_ITCH50
}

case $1 in
    "c")
        clean
        ;;
    "b")
        build
        ;;
    "r")
        run
        ;;
    "all")
        clean
        build
        run
        ;;
    *)
        echo -e "First Arg Options:\nc - clean\nb - build\nr - run\nall - clean, build, run"
        ;;
esac
exit
