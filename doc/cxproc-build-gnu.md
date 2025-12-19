
# How to build cxproc on a GNU/Linux system

## Debian (package names for Bookworm)

Core tools

    sudo apt install debconf debconf-i18n debianutils lintian debhelper g++ cpp gcc gdb make diffutils patch cmake

Optional tools

    sudo apt install valgrind strace splint mc screen wget curl ssh rsync git xsltproc

    sudo apt install cmake-qt-gui kcachegrind gitk emacs meld doxygen graphviz

Core libraries

    sudo apt install libc6-dev libpcre2-dev libarchive-dev zip unzip libzip-dev zlib1g zlib1g-dev libxml2 libxml2-utils libxml2-dev libxslt1-dev libxslt1.1 sqlite3 libsqlite3-dev liblzma-dev libbz2-dev duktape duktape-dev libssl-dev libcurl4-openssl-dev

Graphic libraries (optional)

    sudo apt install imagemagick libmagickcore-dev libgif-dev libgif7 libexif12 libexif-dev libjpeg8 libjpeg8-dev libpng16-16 libpng-dev libtiff5 libtiff5-dev

Audio libraries (optional)

    sudo apt install libid3tag0-dev libogg-dev libvorbis-dev

Network runtime environment

	sudo apt install apache2 apache2-utils samba samba-common smbclient cifs-utils

## Ubuntu (package names for Focal Fossa)

Core tools

    sudo apt install debconf debconf-i18n debianutils lintian debhelper g++ cpp gcc gdb make diffutils patch xsltproc cmake cmake-qt-gui

Core libraries

    sudo apt install libc6-dev libpcre2-dev libarchive-dev zip unzip libzip-dev zlib1g zlib1g-dev libxml2 libxml2-utils libxml2-dev libxslt1-dev libxslt1.1 sqlite3 libsqlite3-dev liblzma-dev libbz2-dev duktape duktape-dev libssl-dev libcurl4-openssl-dev

Graphic libraries (optional)

    sudo apt install imagemagick libmagickcore-dev libgif-dev libgif7 libexif12 libexif-dev libjpeg8 libjpeg8-dev libpng16-16 libpng-dev libtiff5 libtiff5-dev

Audio libraries (optional)

    sudo apt install libid3tag0-dev libogg-dev libvorbis-dev

Development environment

    sudo apt install apache2 apache2-utils doxygen graphviz valgrind kcachegrind strace splint mc screen wget curl ssh rsync meld git gitk emacs samba samba-common smbclient cifs-utils

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

    sudo dnf install pcre2 pcre2-devel zip unzip libzip-devel libzip zlib zlib-devel libxml2 libxml2-devel libxslt libxslt-devel sqlite sqlite-devel libarchive libarchive-devel curl-devel

Graphic libraries

    sudo dnf install ImageMagick ImageMagick-devel libgif4 libgif-devel libexif libexif-devel libjpeg libjpeg-devel libpng12 libpng12-devel libtiff

Audio libraries

    sudo dnf install libid3tag-devel libogg-devel libvorbis-devel

Development environment

    sudo dnf install httpd emacs doxygen graphviz valgrind mc screen wget curl rsync meld git gitk samba-client samba-common cifs-utils openssh openssh-devel

## Samba Share

	

## Git

	mkdir -p ~/cxproc-build/
	cd ~/cxproc-build/
	git clone https://github.com/raxdne/cxproc.git
	cd ~/cxproc-build/cxproc
	git submodule update --remote
	git switch develop

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
	# (cd ~/cxproc-build/cmark/ && git pull)
	mkdir -p ~/cxproc-build/cmark/build
	(cd ~/cxproc-build/cmark/build && cmake .. "-GUnix Makefiles" -DCMAKE_BUILD_TYPE=Release)
	cmake --build ~/cxproc-build/cmark/build
	cmake --install ~/cxproc-build/cmark/build --prefix $PREFIX/

### Duktape as Source code

	wget https://bootstrap.pypa.io/pip/2.7/get-pip.py
	python2 get-pip.py
	python2 -m pip install pyyaml
	
	cd ~/cxproc-build
	wget -c https://duktape.org/duktape-2.7.0.tar.xz
	tar xzf duktape-2.7.0.tar.xz
	cd duktape-2.7.0
	python tools/configure.py --output-directory ~/cxproc-build/cxproc/third-party/duktape-src -DDUK_USE_FASTINT -UDUK_USE_ES6_PROXY

select `~/cxproc-build/cxproc/third-party/duktape-src` as value for `DUKTAPE_INCLUDE_DIR`

### sqlite as Source code

## Debugging

### libxml2

	(cd ~/cxproc-build/ && wget https://download.gnome.org/sources/libxml2/2.9/libxml2-2.9.14.tar.xz && tar xJf libxml2-2.9.14.tar.xz)
	(cd ~/cxproc-build/ && mkdir -p x86_64-gnu-linux-debug && cd libxml2-2.9.14 && sh ./configure --prefix=`pwd`/../x86_64-gnu-linux-debug --with-debug --with-c14n --with-catalog --with-ftp --with-html --with-http --with-iconv --without-icu --without-legacy --without-mem-debug --with-output --with-pattern --with-push --without-python --with-reader --with-regexps --without-run-debug --with-sax1 --with-schemas --with-schematron --with-tree --with-valid --with-writer --with-xinclude --with-xpath --with-xptr --without-modules --without-lzma --without-zlib && make -j 4 && make install)

### libxslt

	(cd ~/cxproc-build/ && wget https://download.gnome.org/sources/libxslt/1.1/libxslt-1.1.35.tar.xz && tar xJf libxslt-1.1.35.tar.xz)
	(cd ~/cxproc-build/ && mkdir -p x86_64-gnu-linux-debug && cd libxslt-1.1.35 && sh ./configure --prefix=`pwd`/../x86_64-gnu-linux-debug --without-python --with-libxml-src=../libxml2-2.9.14 && make -j 4 && make install)

### libarchive

	(cd ~/cxproc-build/ && wget http://www.libarchive.org/downloads/libarchive-3.8.3.tar.xz && tar xJf libarchive-3.8.3.tar.xz)
	(cd ~/cxproc-build/ && mkdir -p x86_64-gnu-linux-debug && cd libarchive-3.8.3 && cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS:BOOL=OFF -DENABLE_TEST:BOOL=OFF -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/../x86_64-gnu-linux-debug . && make -j 4 && make install)

### curl

	(cd ~/cxproc-build/ && wget https://curl.se/download/curl-8.17.0.tar.xz && tar xJf curl-8.17.0.tar.xz)
	(cd ~/cxproc-build/ && mkdir -p x86_64-gnu-linux-debug && cd curl-8.17.0 && cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS:BOOL=OFF -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/../x86_64-gnu-linux-debug . && make -j 4 && make install)

### pcre2

	(cd ~/cxproc-build/ && wget https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.47/pcre2-10.47.tar.bz2 && tar xjf pcre2-10.47.tar.bz2)
	(cd ~/cxproc-build/ && mkdir -p x86_64-gnu-linux-debug && cd pcre2-10.47 && cmake -G "Unix Makefiles" -DBUILD_SHARED_LIBS:BOOL=OFF -DPCRE2_BUILD_PCRE2GREP:BOOL=OFF -DPCRE2_BUILD_TESTS:BOOL=OFF -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/../x86_64-gnu-linux-debug . && make -j 4 && make install)

### duktape

	(cd ~/cxproc-build/ && wget https://duktape.org/duktape-2.7.0.tar.xz && tar xJf duktape-2.7.0.tar.xz)
	(cd ~/cxproc-build/ && mkdir -p x86_64-gnu-linux-debug && cd duktape-2.7.0 && make -j 4 -f Makefile.sharedlibrary install INSTALL_PREFIX=`pwd`/../x86_64-gnu-linux-debug)

### using valgrind

	(cd ~/cxproc-build/cxproc/test/xml/ && valgrind --tool=memcheck --leak-check=full --fullpath-after= --num-callers=20 ~/cxproc-build/x86_64-gnu-linux/bin/cxproc config-xml-subst.cxp &> ~/cxproc-build/x86_64-gnu-linux/memcheck.log)

## CMake

build directory is defined by `misc/prepare-cmake.sh`

	cd ~/cxproc-build/cxproc
	. misc/prepare-cmake.sh
	#cmake -S `pwd` -B $DIR_BUILD -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Debug -DCXPROC_DOC:BOOL=OFF -DCXPROC_LEGACY:BOOL=OFF -DCXPROC_EXPERIMENTAL:BOOL=ON -DCXPROC_MARKDOWN:BOOL=OFF -DCXPROC_PCRE2:BOOL=OFF -DCXPROC_PETRINET:BOOL=OFF -DCXPROC_PIE:BOOL=OFF -DCXPROC_ARCHIVE:BOOL=OFF -DCXPROC_CURL:BOOL=OFF -DCXPROC_DUKTAPE:BOOL=OFF -DCXPROC_JSON:BOOL=OFF -DCXPROC_SQLITE3:BOOL=OFF -DCXPROC_TESTS:BOOL=ON
	cmake -S `pwd` -B $DIR_BUILD -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -DCXPROC_DOC:BOOL=OFF -DCXPROC_LEGACY:BOOL=ON -DCXPROC_EXPERIMENTAL:BOOL=ON -DCXPROC_MARKDOWN:BOOL=ON -DCXPROC_ARCHIVE:BOOL=ON -DCXPROC_CURL:BOOL=ON -DCXPROC_DUKTAPE:BOOL=ON -DCXPROC_SQLITE3:BOOL=ON
	# cmake-gui -S `pwd` -B $DIR_BUILD &
	cmake --build $DIR_BUILD -j 4 --target all
	(cd $DIR_BUILD && ctest)
	cmake --install $DIR_BUILD --prefix $PREFIX/
	#cmake --build $DIR_BUILD --target package
	#(cd $DIR_BUILD && cpack -V --debug)
	cmake -B $DIR_BUILD --target clean

GUI

## Doxygen

    doxygen misc/Doxyfile

## Cross-compiling

https://askubuntu.com/questions/250696/how-to-cross-compile-for-arm

	sudo apt-get install gcc-arm-linux-gnueabihf g++-arm-linux-gnueabihf
	
## Apache Webserver runtime

	sudo a2enmod cgi actions
	sudo systemctl reload apache2
	curl 'http://localhost/cxproc/exe'

PIE

	cd $PREFIX/www/html && git clone https://github.com/raxdne/pie.git
	cp -r $PREFIX/www/html/pie/test .
	sudo chgrp -R www-data $PREFIX/www

jQuery

tablesorter

ACE

### Virtualhost + [suexec](https://httpd.apache.org/docs/trunk/suexec.html)

When suexec is used to launch CGI scripts, the environment will be cleaned down to a set of safe variables before CGI scripts are launched. The list of safe variables is defined at compile-time in `suexec.c`.

use of Apache `suexec` on Debian GNU/Linux

	sudo apt install apache2-suexec-custom
	ls -l /usr/lib/apache2/suexec-custom
	/usr/lib/apache2/suexec -V
	sudo a2enmod suexec
	sudo systemctl restart apache2

To define suexec document root and the suexec userdir suffix insert into `/etc/apache2/suexec/www-data`

	/srv

In `/etc/apache2/apache2.conf` turn

	Suexec On

and in VirtualHost

	SuexecUserGroup developer developer

TODO: Suggested setup

	PREFIX=/tmp ; for N in test john krita ; do (mkdir -p $PREFIX/srv/$N/cgi-bin $PREFIX/srv/$N/Notes ; cp /etc/apache2/sites-available/cxproc-pie-test.conf $PREFIX/srv/$N/cxproc-pie-$N.conf ; cp /srv/www/cgi-bin/cxproc-cgi $PREFIX/srv/$N/cgi-bin ; cp /srv/www/test/index.html $PREFIX/srv/$N/Notes/ ; chown -R $N:$N $PREFIX/srv/$N) ; done ; (cd $PREFIX ; zip -r srv-cxproc srv )

### [userdir](https://httpd.apache.org/docs/2.4/howto/public_html.html)

is not a good option, due to public and generic approach

## Packaging

### Debian/Ubuntu

	cd ~/cxproc-build/cxproc
	sudo dpkg-buildpackage
	dpkg --dry-run -i ../cxproc_2.0-rc2-1_amd64.deb
	sudo rm -r debian/.debhelper/ debian/cxproc debian/cxproc.substvars debian/files

### Fedora

