#!/bin/sh
#
# (p) 2016,2020 A. Tenbusch
#

DEBUG=1

# Required tools: git, CMake

PREFIX=$(uname -i)

PREFIX=../$PREFIX"-"$(uname -o | tr '[:upper:]' '[:lower:]' | tr '[:punct:]' '-')

if [ $DEBUG = 1 ] ; then
  PREFIX=$PREFIX-debug
fi

test -d $PREFIX || mkdir -p $PREFIX

export DIR_BIN="$PREFIX/bin"
test -d $DIR_BIN || mkdir -p $DIR_BIN
export DIR_BUILD="$PREFIX/build"
test -d $DIR_BUILD || mkdir -p $DIR_BUILD
export DIR_LIB="$PREFIX/lib"
test -d $DIR_LIB || mkdir -p $DIR_LIB
export DIR_INC="$PREFIX/include"
test -d $DIR_INC || mkdir -p $DIR_INC
export DIR_DOC="$PREFIX/doc"
test -d $DIR_DOC || mkdir -p $DIR_DOC
export DIR_TEST="$PREFIX/test"
test -d $DIR_TEST || cp -r test $DIR_TEST
#export DIR_DATA="$PREFIX/data"
#test -d $DIR_DATA || cp -r www/Test $DIR_DATA
export DIR_CGI="$PREFIX/www/cgi-bin"
test -d $DIR_CGI || mkdir -p $DIR_CGI
export DIR_LOG="$PREFIX/www/log"
test -d $DIR_LOG || mkdir -p $DIR_LOG
export DIR_WWW="$PREFIX/www/html"
test -d $DIR_WWW || mkdir -p $DIR_WWW

#cmake-gui build/cmake &

#(cd $PREFIX && cmake ..)
