#!/usr/bin/bash
#
#

FLAG_CONFIG=1
FLAG_CGI=0
N=5

TEST_ROOT=~/cxproc-build/cxproc
test -d $TEST_ROOT || mkdir -p $TEST_ROOT
# git clone https://github.com/raxdne/cxproc.git
# git submodule update --remote

ARCH=$(uname -m)"-"$(uname -o | tr '[:upper:]' '[:lower:]' | tr '[:punct:]' '-')

#REVS=$(git rev-list --branches --max-count=$N)
#REVS=ecde184c9d4ae3dd5fe560bd1387c6d7ee53e10f
REVS=develop

#(cd $TEST_ROOT && git checkout develop)

#(cd $TEST_ROOT/../$ARCH/www/html/pie && git checkout develop)

I=0
for r in $REVS
do
    #PREFIX=$TEST_ROOT/../$ARCH/$r
    #PREFIX=$TEST_ROOT/../$ARCH
    #PREFIX=$TEST_ROOT/../$I-$ARCH
    PREFIX=$TEST_ROOT/../$I-$r

    if [ "$FLAG_CONFIG" == "1" ] ; then
	echo "! preparing '$PREFIX' ..."
	DIR_BUILD="$PREFIX/build"
	test -d $DIR_BUILD || mkdir -p $DIR_BUILD

	git checkout $r
	
	cmake -S $TEST_ROOT -B $DIR_BUILD -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Profile -DCXPROC_PCRE2:BOOL=ON -DCXPROC_DOC:BOOL=OFF -DCXPROC_LEGACY:BOOL=OFF -DCXPROC_EXPERIMENTAL:BOOL=ON -DCXPROC_PIE:BOOL=ON -DCXPROC_MARKDOWN:BOOL=ON -DCXPROC_DUKTAPE:BOOL=ON -DLIBCMARK_LIBRARY:PATH=/home/developer/cxproc-build/x86_64-gnu-linux/lib/libcmark.a  -DLIBCMARK_INCLUDE_DIR:PATH=/home/developer/cxproc-build/x86_64-gnu-linux/include
    fi

    #cmake --build $DIR_BUILD -j 4 --target clean
    cmake --build $DIR_BUILD -j 8 --target cxproc cxproc-cgi
    #(cd $DIR_BUILD && ctest -R basics)

    cd $PREFIX
    
    #export DOCUMENT_ROOT="$DIR_PREFIX/www/html/develop/"
    export DOCUMENT_ROOT=$TEST_ROOT/../$ARCH/www/html/develop
    
    if [[ "$FLAG_CGI" == "1" && -x "www/cgi-bin/cxproc-cgi" ]] ; then
	export REQUEST_METHOD="GET"
	export SERVER_NAME="localhost"
	export SERVER_PORT="8181"
	export SERVER_PROTOCOL="HTTP/1.1"
	export GATEWAY_INTERFACE="CGI/1.1"
	export HTTP_HOST="localhost:8181"

	export CXP_ROOT=$DOCUMENT_ROOT
	export CXP_LOG="4"
	export CXP_LOGFILE=$PREFIX/cxproc.log
	export CXP_PATH=$TEST_ROOT/../$ARCH/www/html/pie//
	#export CXP_DATE="2010"

	#export QUERY_STRING="path=Notes/Main.pie&cxp=PiejQDefault"
	export QUERY_STRING="path=Work/Documents/Main.pie&cxp=PiejQDefault"
	#export QUERY_STRING="cxp=info"
	#export QUERY_STRING="year=2002"
	#export QUERY_STRING="path=/&cxp=MergePie&depth=999"
	#export QUERY_STRING="file=Praesentation.mm&cxp=PresentationIndex&type=application-mm-xml"
	#export QUERY_STRING="cxp=PresentationSlide&i=1&file=Praesentation.mm"
	#export QUERY_STRING="xsl=Hello"
	#export QUERY_STRING="path=Test/Documents/TestContent.txt&cxp=PieUiDefault"
	#export QUERY_STRING="path=Test/Documents/TestContent.txt&cxp=Format&b=text/html%2Bcalendar"
	#export QUERY_STRING="path=Test/Documents/TestContent.txt&cxp=ShowTransformations&b=text/html%2Bcalendar"
	#export QUERY_STRING="cxp=gallery&file=images/20090307-bild.jpg"
	#export QUERY_STRING="cxp=gallery&dir=images"
	#export QUERY_STRING="dir=/&cxp=PieUiDir"
	#export QUERY_STRING="cxp=image&frame=80x80&path=Pictures/IMG_20220320_095854.jpg"

	/usr/bin/time -v -o cxproc.dat www/cgi-bin/cxproc-cgi > cxproc.html 2> cxproc.err
	#valgrind -s  --leak-check=full --tool=memcheck www/cgi-bin/cxproc-cgi > cxproc.xml 2> cxproc.err
	#valgrind --tool=callgrind --log-file=cxproc-cgi-cachegrind.log www/cgi-bin/cxproc-cgi > cxproc.xml 2> cxproc.err
	
    elif [ -x "bin/cxproc" ] ; then
	FILE=$DOCUMENT_ROOT/Notes/Main.pie
	
	#bin/cxproc -e > cxproc.xml 2> cxproc.err
	#/usr/bin/time -v -o cxproc.dat bin/cxproc $FILE > cxproc.xml 2> cxproc.err
	#/usr/bin/time -v -o cxproc.dat
	bin/cxproc $FILE > cxproc.xml 2> cxproc.err
	#valgrind --tool=callgrind --log-file=cxproc-callgrind.log bin/cxproc $FILE > cxproc.xml 2> cxproc.err
	#valgrind -s --leak-check=full --tool=memcheck --log-file=cxproc-memcheck.log bin/cxproc $FILE > cxproc.xml 2> cxproc.err

	# s. https://github.com/jrfonseca/gprof2dot
	# pip3 install gprof2dot
	#gprof bin/cxproc gmon.out | ~/.local/bin/gprof2dot -e0 -n0 --color-nodes-by-selftime | dot -Tsvg -o cxproc.svg
	#gprof bin/cxproc gmon.out | ~/.local/bin/gprof2dot -e0 -n0 --color-nodes-by-selftime -z pieProcessPieNode | dot -Tsvg -o cxproc.svg
	#gprof -b ./cxproc ~/www/gmon.out &> $TMP/cxproc-cgi-profile.log
    fi

    #strace -o cxproc.trace `pwd`/cxproc

    I=`expr $I + 1`

done

#find $TEST_ROOT/../ -type f -iname cxproc.err -print -exec tail -2 {} \;
#find $TEST_ROOT/../ -type f \( -iname 'cxproc.err' \) -exec ls -l {} \;

#find $TEST_ROOT/../ -type f \( -iname 'cxproc.xml' \) -exec ls -l {} \;
#find $TEST_ROOT/../ -type f \( -iname 'cxproc.html' \) -exec ls -l {} \;


# cd $TEST_ROOT && git checkout develop
