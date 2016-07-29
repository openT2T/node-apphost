#!/bin/bash

NORMAL_COLOR='\033[0m'
RED_COLOR='\033[0;31m'
GREEN_COLOR='\033[0;32m'
GRAY_COLOR='\033[0;37m'

LOG() {
  COLOR="$1"
  TEXT="$2"
  echo -e "${COLOR}$TEXT ${NORMAL_COLOR}"
}

cp jni/Android.mk.default jni/Android.mk

if [ $# -eq 1 ]
then
  rm -rf jxcore-binaries/
  mkdir jxcore-binaries/
  cp -R ../../../out/jxcore_android_ia32/* jxcore-binaries/
  if [ $? != 0 ]; then
    echo "couldn't find the android binaries."
    echo "Did you compile android binaries already ?"
    echo ""
    echo "node build.js --platform=android --ndk-path=[ndk-path here]"
    exit 1
  fi
  rm jxcore-binaries/*_x64.a
else
  LOG $RED_COLOR "build.sh [android ndk path]"
  exit
fi

$1/ndk-build

if [ $? != 0 ]; then
  exit 1
fi

# try to update android project
mkdir -p ../libs/armeabi/
mkdir -p ../libs/armeabi-v7a/
mkdir -p ../libs/x86/

OUT=$(cp libs/armeabi/* ../libs/armeabi/)
OUT=$(cp libs/armeabi-v7a/* ../libs/armeabi-v7a/)
OUT=$(cp libs/x86/* ../libs/x86/)

rm -rf jxcore-binaries/
rm -rf obj/
