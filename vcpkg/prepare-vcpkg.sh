#!/bin/sh
#
# (p) 2026 A. Tenbusch
#

# https://stackoverflow.com/questions/15986715/how-do-i-invoke-the-mingw-cross-compiler-on-linux
# sudo apt install mingw-w64

# check https://learn.microsoft.com/en-us/vcpkg/

# https://learn.microsoft.com/en-us/vcpkg/concepts/triplets
PREFIX_VCPKG=~/cxproc-build
#ARCH=$(uname -m)"-"$(uname -o | tr '[:upper:]' '[:lower:]' | tr '[:punct:]' '-')
#ARCH=x64-mingw-dynamic
ARCH=x64-linux
PREFIX=$PREFIX_VCPKG/$ARCH

echo "! preparing '$PREFIX' ..."

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

cd $PREFIX_VCPKG

# https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-packaging?pivots=shell-bash

if [ -d $PREFIX_VCPKG/vcpkg ] ; then 
  cd vcpkg 
  git pull
else
  git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg 
fi

rm status
git checkout 2026.03.18
./bootstrap-vcpkg.sh -disableMetrics

./vcpkg integrate install

./vcpkg --triplet $ARCH --x-install-root=$PREFIX_VCPKG --recurse install zlib libzip liblzma libarchive pcre2 libxml2[tools] libxslt[tools] sqlite3 duktape cmark[tools] curl[tool] libexif
# imagemagick libgif libjpeg libpng libtiff

# cmake -DCMAKE_TOOLCHAIN_FILE=~/cxproc-build/vcpkg/scripts/buildsystems/vcpkg.cmake -S ~/cxproc-build/cxproc -B $DIR_BUILD -DCMAKE_BUILD_TYPE=Debug -DCXPROC_DOC:BOOL=OFF -DCXPROC_LEGACY:BOOL=OFF -DCXPROC_EXPERIMENTAL:BOOL=ON -DCXPROC_MARKDOWN:BOOL=OFF -DCXPROC_PCRE2:BOOL=OFF -DCXPROC_PETRINET:BOOL=OFF -DCXPROC_PIE:BOOL=OFF -DCXPROC_ARCHIVE:BOOL=OFF -DCXPROC_CURL:BOOL=OFF -DCXPROC_DUKTAPE:BOOL=OFF -DCXPROC_JSON:BOOL=OFF -DCXPROC_SQLITE3:BOOL=OFF -DCXPROC_TESTS:BOOL=ON

# cmake -DCMAKE_TOOLCHAIN_FILE=~/cxproc-build/cxproc/cmake/mingw-w64-x86_64.cmake -S ~/cxproc-build/cxproc -B $DIR_BUILD -DCMAKE_BUILD_TYPE=Debug -DCXPROC_DOC:BOOL=OFF -DCXPROC_LEGACY:BOOL=OFF -DCXPROC_EXPERIMENTAL:BOOL=ON -DCXPROC_MARKDOWN:BOOL=OFF -DCXPROC_PCRE2:BOOL=OFF -DCXPROC_PETRINET:BOOL=OFF -DCXPROC_PIE:BOOL=OFF -DCXPROC_ARCHIVE:BOOL=OFF -DCXPROC_CURL:BOOL=OFF -DCXPROC_DUKTAPE:BOOL=OFF -DCXPROC_JSON:BOOL=OFF -DCXPROC_SQLITE3:BOOL=OFF -DCXPROC_TESTS:BOOL=ON

# cmake --build $DIR_BUILD -j 4 --target all
# cxproc filex cxproc-cgi cxproc-test 
