#! /bin/bash

# The script takes one argument, which is the team ID
# Ensure the team ID is a non-empty string

if [ -z "$1" ]
then
    echo "Usage: $0 <team_id>"
    exit 1
fi

cmake -Wno-dev --warn-uninitialized -S . -B "./build/" -G "Xcode" \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
  -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=$1 \
  -DCMAKE_SYSTEM_NAME=iOS \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0
