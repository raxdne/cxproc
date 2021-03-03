#!/bin/sh
#
# (p) 2016,2020 A. Tenbusch
#

# rsync -avC ~/cxproc-build/cxproc/ developer@cubietruck-dev:cxproc-build/cxproc/

#ARCH=x64-linux-static
#ARCH=arm-linux-static
ARCH=$(uname -i)"-"$(uname -o | tr '[:upper:]' '[:lower:]' | tr '[:punct:]' '-')

PREFIX=../$ARCH

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

#
# use of VCPKG on Linux
#
# sudo apt install libtool autoconf python-yaml
# cd ..
# git clone https://github.com/microsoft/vcpkg.git
# cd vcpkg
# echo -e "set(VCPKG_TARGET_ARCHITECTURE x64)\nset(VCPKG_CRT_LINKAGE dynamic)\nset(VCPKG_LIBRARY_LINKAGE static)\nset(VCPKG_ENV_PASSTHROUGH PATH)\nset(VCPKG_CMAKE_SYSTEM_NAME Linux)\n" > triplets/community/x64-linux-static.cmake
# echo -e "set(VCPKG_TARGET_ARCHITECTURE arm)\nset(VCPKG_CRT_LINKAGE dynamic)\nset(VCPKG_LIBRARY_LINKAGE static)\nset(VCPKG_ENV_PASSTHROUGH PATH)\nset(VCPKG_CMAKE_SYSTEM_NAME Linux)\n" > triplets/community/arm-linux-static.cmake
# ./bootstrap-vcpkg.sh
# ./vcpkg integrate install
# ./vcpkg help triplet
# ./vcpkg --triplet $ARCH --x-install-root=.. install zlib liblzma pcre2 libxml2 libxslt libexif sqlite3 duktape

# https://stackoverflow.com/questions/58777810/how-to-integrate-vcpkg-in-linux-with-cross-build-toolchain-as-well-as-sysroot

