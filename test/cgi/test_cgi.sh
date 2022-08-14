#
#
#

DIR_PREFIX=/home/developer/cxproc-build/x86_64-gnu-linux/

export REQUEST_METHOD="GET"
export SERVER_NAME="localhost"
export SERVER_PORT="8181"
export SERVER_PROTOCOL="HTTP/1.1"
export GATEWAY_INTERFACE="CGI/1.1"
export HTTP_HOST="localhost:8181"

export DOCUMENT_ROOT="$DIR_PREFIX/www/html/develop/"
export CXP_ROOT=$DOCUMENT_ROOT
export CXP_PATH="$DIR_PREFIX/www/html/pie//"
export CXP_BACKUPS="Backup/"
export CXP_TEMPLATES="Templates/"
export CXP_LOG="4"
#export CXP_DATE="2010"

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

TMP=$DIR_PREFIX/tmp

test -d $TMP || mkdir $TMP

export CXP_LOGFILE="$TMP/cxproc-cgi.log"

#$DIR_PREFIX/www/cgi-bin/cxproc-cgi 1> $TMP/cxproc-cgi.txt 2> $TMP/cxproc-cgi.err

valgrind --leak-check=full --show-error-list=yes --show-reachable=yes --show-leak-kinds=all --log-file=$TMP/cxproc-cgi-valgrind.log $DIR_PREFIX/www/cgi-bin/cxproc-cgi 1> $TMP/cxproc-cgi.txt 2> $TMP/cxproc-cgi.err

#valgrind --tool=callgrind --callgrind-out-file=$TMP/cxproc-cgi-callgrind.log --log-file=$TMP/cxproc-cgi-valgrind.log $DIR_PREFIX/www/cgi-bin/cxproc-cgi 1> $TMP/cxproc-cgi.txt 2> $TMP/cxproc-cgi.err

#valgrind --tool=cachegrind --cachegrind-out-file=$TMP/cxproc-cgi-cachegrind.log --log-file=$TMP/cxproc-cgi-valgrind.log $DIR_PREFIX/www/cgi-bin/cxproc-cgi 1> $TMP/cxproc-cgi.txt 2> $TMP/cxproc-cgi.err

# s. http://code.google.com/p/jrfonseca/wiki/Gprof2Dot
#gprof ./cxproc ~/www/gmon.out | ~/Desktop/gprof2dot.py | dot -Tpng -o output.png
#gprof -b ./cxproc ~/www/gmon.out &> $TMP/cxproc-cgi-profile.log

#strace -o cxproc.trace `pwd`/cxproc

