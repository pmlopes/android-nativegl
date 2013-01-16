#!/bin/sh
NDK=`which ndk-build`
NDK=`dirname $NDK`
$NDK/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-addr2line -C -f -e ../obj/local/armeabi/libna.so $1
