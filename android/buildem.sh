#!/bin/sh -e

ORIG=$PWD
INSTALL=$PWD/deps

SSL=$PWD/openssl-1.0.2
BDB=$PWD/db-5.3.28
TOOLS=$PWD/arm-linux-androideable-4.8
FAST=-j4

#have to do this first or bdb gets confused?
#note: tools comes from make-standalone-toolchain.sh to have a correct sysroot
#see https://code.google.com/p/android/issues/detail?id=39915
export PATH=$TOOLS/bin:$PATH

cd $BDB
cd build_unix
../dist/configure --enable-cxx --host=arm-linux-androideabi --build=x86_64-pc-linux-gnu --prefix=$INSTALL
make $FAST
make install

cd $SSL
. $ORIG/Setenv-android.sh 
perl -pi -e 's/install: all install_docs install_sw/install: install_docs install_sw/g' Makefile.org
./config shared no-ssl2 no-ssl3 no-comp no-hw no-engine --openssldir=$INSTALL

make $FAST depend 
make $FAST all
make install

#TODO: use https://github.com/MysticTreeGames/Boost-for-Android
