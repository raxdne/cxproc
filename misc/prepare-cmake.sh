#!/bin/sh
#
# (p) 2016,2020 A. Tenbusch
#

DEBUG=1

PREFIX=../$(uname -i)

PREFIX=$PREFIX"-"$(uname -o | tr '[:upper:]' '[:lower:]' | tr '[:punct:]' '-')

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
export DIR_TMP="$PREFIX/tmp"
test -d $DIR_TMP || mkdir -p $DIR_TMP

export DIR_CGI="$PREFIX/www/cgi-bin"
test -d $DIR_CGI || mkdir -p $DIR_CGI
export DIR_LOG="$PREFIX/www/log"
test -d $DIR_LOG || mkdir -p $DIR_LOG
#export DIR_CONF="$PREFIX/www/conf"
#test -d $DIR_CONF || mkdir -p $DIR_CONF
export DIR_WWW="$PREFIX/www/html"
test -d $DIR_WWW/test || mkdir -p $DIR_WWW/test
