
# How to build cxproc on a GNU/Linux system

## Ubuntu

Core tools

    sudo apt install debconf debconf-i18n debianutils lintian debhelper g++ cpp gcc gdb make diffutils patch xsltproc cmake cmake-qt-gui

Core libraries

    sudo apt install libc6-dev libpcre2-dev libarchive-dev zip unzip libzip-dev zlib1g zlib1g-dev libxml2 libxml2-utils libxml2-dbg libxml2-dev libxslt1-dbg libxslt1-dev libxslt1.1 sqlite3 libsqlite3-dev liblzma-dev libbz2-dev duktape duktape-dev

Graphic libraries (optional)

    sudo apt install imagemagick libmagickcore-dev libgif-dev libgif7 libexif12 libexif-dev libjpeg8 libjpeg8-dev libpng12-0 libpng12-dev libtiff5 libtiff5-dev

Audio libraries (optional)

    sudo apt install libid3tag0-dev libogg-dev libvorbis-dev

Development environment

    sudo apt install apache2 apache2-utils doxygen graphviz valgrind kcachegrind strace splint cflow mc screen w3m wget ssh rsync meld git emacs samba samba-common smbclient cifs-utils

## Fedora (package names are not up-to-date)

s. <http://rpmfind.net/linux/rpm2html/search.php>

Core tools

    sudo dnf install cpp gcc gcc-c++ gdb make strace patch pkg-config gawk cmake cmake-gui

additional core libraries

    sudo dnf install pcre2 pcre2-devel zip unzip libzip-devel libzip zlib zlib-devel libxml2 libxml2-devel libxslt libxslt-devel sqlite sqlite-devel libarchive libarchive-devel

Graphic libraries

    sudo dnf install ImageMagick ImageMagick-devel libgif4 libgif-devel libexif libexif-devel libjpeg libjpeg-devel libpng12 libpng12-devel libtiff

Audio libraries

    sudo dnf install libid3tag-devel libogg-devel libvorbis-devel

Development environment

    sudo dnf install httpd emacs doxygen graphviz graphviz-doc valgrind kcachegrind splint cflow mc screen w3m wget ssh rsync 

## Git

    git clone https://github.com/raxdne/cxproc.git

## CMake

build directory is defined by `misc/prepare-cmake.sh`

	cd cxproc
	. misc/prepare-cmake.sh
	(cd $PREFIX/build && cmake ~/cxproc-build/cxproc/ "-GUnix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCXPROC_DOC:BOOL=OFF -DCXPROC_EXPERIMENTAL:BOOL=OFF)
	cmake --build $PREFIX/build
	(cd $PREFIX/build && ctest)
	cmake --build $PREFIX/build --target package

GUI

## Doxygen

    doxygen misc/Doxyfile

## Cross-compiling

https://askubuntu.com/questions/250696/how-to-cross-compile-for-arm

	sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
	
