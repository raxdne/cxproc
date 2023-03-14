
# How to build cxproc on a GNU/Linux system

## Ubuntu (package names for Focal Fossa)

Core tools

    sudo apt install debconf debconf-i18n debianutils lintian debhelper g++ cpp gcc gdb make diffutils patch xsltproc cmake cmake-qt-gui

Core libraries

    sudo apt install libc6-dev libpcre2-dev libarchive-dev zip unzip libzip-dev zlib1g zlib1g-dev libxml2 libxml2-utils libxml2-dev libxslt1-dev libxslt1.1 sqlite3 libsqlite3-dev liblzma-dev libbz2-dev duktape duktape-dev libssl-dev curl libcurl4-openssl-dev

Graphic libraries (optional)

    sudo apt install imagemagick libmagickcore-dev libgif-dev libgif7 libexif12 libexif-dev libjpeg8 libjpeg8-dev libpng16-16 libpng-dev libtiff5 libtiff5-dev

Audio libraries (optional)

    sudo apt install libid3tag0-dev libogg-dev libvorbis-dev

Development environment

    sudo apt install apache2 apache2-utils doxygen graphviz valgrind kcachegrind strace splint cflow mc screen w3m wget ssh rsync meld git gitk emacs samba samba-common smbclient cifs-utils

to find missing libraries or header files on Debian/Ubuntu

	sudo apt install apt-file
	sudo apt-file update
	apt-file find  ImageMagick.h

## Fedora

s. <http://rpmfind.net/linux/rpm2html/search.php>

Don't use packages `cmark cmark-devel`

Core tools

    sudo dnf install cpp gcc gcc-c++ gdb make patch pkg-config gawk cmake cmake-gui

additional core libraries

    sudo dnf install pcre2 pcre2-devel zip unzip libzip-devel libzip zlib zlib-devel libxml2 libxml2-devel libxslt libxslt-devel sqlite sqlite-devel libarchive libarchive-devel curl curl-devel

Graphic libraries

    sudo dnf install ImageMagick ImageMagick-devel libgif4 libgif-devel libexif libexif-devel libjpeg libjpeg-devel libpng12 libpng12-devel libtiff

Audio libraries

    sudo dnf install libid3tag-devel libogg-devel libvorbis-devel

Development environment

    sudo dnf install httpd emacs doxygen graphviz valgrind mc screen w3m wget rsync meld git gitk samba-client samba-common cifs-utils openssh openssh-devel

## Samba Share

	

## Git

	mkdir -p ~/cxproc-build/
	cd ~/cxproc-build/
    git clone https://github.com/raxdne/cxproc.git
	cd ~/cxproc-build/cxproc
    git submodule update --remote

## CMake

build directory is defined by `misc/prepare-cmake.sh`

	cd ~/cxproc-build/cxproc
	. misc/prepare-cmake.sh
	(cd $PREFIX/build && cmake ~/cxproc-build/cxproc/ "-GUnix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCXPROC_DOC:BOOL=OFF -DCXPROC_LEGACY:BOOL=ON -DCXPROC_EXPERIMENTAL:BOOL=ON -DCXPROC_MARKDOWN:BOOL=OFF)
	cmake --build $PREFIX/build -j 4
	(cd $PREFIX/build && ctest)
	cmake --install $PREFIX/build --prefix $PREFIX/
	#cmake --build $PREFIX/build --target package
	#(cd $PREFIX/build && cpack -V --debug)
	cmake --build $PREFIX/build --target clean

GUI

## third party

### c-dt

	cd ~/cxproc-build/cxproc
	git submodule update --init third-party/c-dt

### cmark

	cd ~/cxproc-build/cxproc
	. misc/prepare-cmake.sh
	cd ~/cxproc-build/
	# sudo dpkg --remove cmark libcmark0.29.0 libcmark-dev
	git clone https://github.com/commonmark/cmark.git
	mkdir -p ~/cxproc-build/cmark/build
	(cd ~/cxproc-build/cmark/build && cmake .. "-GUnix Makefiles" -DCMAKE_BUILD_TYPE=Release)
	cmake --build ~/cxproc-build/cmark/build
	cmake --install ~/cxproc-build/cmark/build --prefix $PREFIX/

## Doxygen

    doxygen misc/Doxyfile

## Cross-compiling

https://askubuntu.com/questions/250696/how-to-cross-compile-for-arm

	sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
	
## Webserver runtime

	sudo a2enmod cgi actions
	sudo systemctl reload apache2
	curl 'http://localhost/cgi-bin/cxproc-cgi?path=test'
	
	cd $PREFIX/www/html && git clone https://github.com/raxdne/pie.git
	cp -r $PREFIX/www/html/pie/test .
	sudo chgrp -R www-data $PREFIX/www
	
## Packaging

### Debian/Ubuntu

	cd ~/cxproc-build/cxproc
	sudo dpkg-buildpackage
	dpkg --dry-run -i ../cxproc_2.0-rc2-1_amd64.deb
	sudo rm -r debian/.debhelper/ debian/cxproc debian/cxproc.substvars debian/files

### Fedora

