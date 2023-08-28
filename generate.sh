#! /bin/bash
cmake -Wno-dev --warn-uninitialized -S . -B "./build/" -G "Xcode" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"