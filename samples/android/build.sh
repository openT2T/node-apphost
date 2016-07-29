#!/bin/bash

if [ $# -eq 1 ]
then
  cd JNI_JXCORE
  RET=$(./build_libs.sh $1)
  if [ $? != 0 ]; then
    echo -e "${RET}"
    exit 1
  fi
  echo "Native Binaries are successfully updated"
else
  echo "build.sh [android ndk path]"
  exit 1
fi
