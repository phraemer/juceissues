#! /bin/bash
cmake -Wno-dev --warn-uninitialized -S . -B "./build/" -G "Xcode" -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 -DBUILD_IOS=ON