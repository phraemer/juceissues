#! /bin/bash
cmake -Wno-dev --warn-uninitialized -S . -B "./build/" -G "Xcode"