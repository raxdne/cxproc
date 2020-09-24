#!/bin/sh
#
# (p) 2016,2020 A. Tenbusch
#

DEBUG=1

# Required tools: git, CMake, 7-ZIP

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
export DIR_TEST="$PREFIX/test"
export DIR_DATA="$PREFIX/data"
export DIR_TMP="$PREFIX/tmp"
test -d $DIR_TMP || mkdir -p $DIR_TMP
export DIR_CGI="$PREFIX/www/cgi-bin"
test -d $DIR_CGI || mkdir -p $DIR_CGI
export DIR_LOG="$PREFIX/www/log"
test -d $DIR_LOG || mkdir -p $DIR_LOG
export DIR_WWW="$PREFIX/www/html"
test -d $DIR_WWW || mkdir -p $DIR_WWW

#test -f build/jquery-urls.txt && wget -c --directory-prefix=$DIR_WWW --force-directories -i build/jquery-urls.txt

#test -d $DIR_WWW/pie || svn co http://arrakis.fritz.box:8187/www/html/trunk $DIR_WWW/pie

#test -d $DIR_WWW/Documents || svn co http://arrakis.fritz.box:8187/cxproc/trunk/contrib/pie/www $DIR_WWW/Documents

#test -d $DIR_TEST || svn co http://arrakis.fritz.box:8187/cxproc/trunk/test $DIR_TEST

#test -d $DIR_DATA || svn co http://arrakis.fritz.box:8187/cxproc/trunk/contrib/pie/www/Test $DIR_DATA

#test -d src || ln -s ../trunk/src

if [ ! -d $PREFIX/etc ] ; then
    mkdir -p $PREFIX/etc/apache2/sites-available
    #svn export http://arrakis.fritz.box:8187/cxproc/trunk/contrib/pie/browser-jquery/cxproc-jquery.conf $PREFIX/etc/apache2/sites-available/
fi

#cmake-gui build/cmake &

#(cd $PREFIX && cmake ..)
