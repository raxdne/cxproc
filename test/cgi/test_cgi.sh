#
#
#

export CXP_BACKUPS="Backup/"
export CXP_LOG="4"
#export CXP_DATE="2010"
export CXP_ROOTAUDIO="/home/developer/"
export CXP_ROOT="/home/developer/cxproc-build/trunk/contrib/pie/www"
export CXP_TEMPLATES="Templates/"
export DOCUMENT_ROOT="/home/developer/cxproc-build/trunk/contrib/pie/www/"
export GATEWAY_INTERFACE="CGI/1.1"
export HTTP_HOST="localhost:8181"
export CXP_PATH="/home/developer/cxproc-build/trunk//"
#export QUERY_STRING="cxp=info"
#export QUERY_STRING="path=/&cxp=MergePie&depth=999"
#export QUERY_STRING="file=Praesentation.mm&cxp=PresentationIndex&type=application-mm-xml"
#export QUERY_STRING="cxp=PresentationSlide&i=1&file=Praesentation.mm"
#export QUERY_STRING="xsl=Hello"
export QUERY_STRING="path=Test/Documents/TestContent.txt&cxp=PieUiDefault"
#export QUERY_STRING="cxp=gallery&file=images/20090307-bild.jpg"
#export QUERY_STRING="cxp=gallery&dir=images"
#export QUERY_STRING="dir=/&cxp=PieUiDir"
export REQUEST_METHOD="GET"
export SERVER_NAME="localhost"
export SERVER_PORT="8181"
export SERVER_PROTOCOL="HTTP/1.1"

TMP=tmp

test -d $TMP || mkdir $TMP

export CXP_LOGFILE="$TMP/cxproc-cgi.log"

valgrind --leak-check=full --show-reachable=yes `pwd`/www/cgi-bin/cxproc-cgi 1> $TMP/cxproc-cgi.txt 2> $TMP/cxproc-cgi-valgrind.log
#`pwd`/cxproc 1> $TMP/cxproc-cgi.txt 2> $TMP/cxproc-cgi.log
#gprof -b ./cxproc ~/www/gmon.out &> $TMP/cxproc-cgi-profile.log
# s. http://code.google.com/p/jrfonseca/wiki/Gprof2Dot
#gprof ./cxproc ~/www/gmon.out | ~/Desktop/gprof2dot.py | dot -Tpng -o output.png
#`pwd`/cxproc 2> tmp/cxproc-cgi.log
#`pwd`/cxproc > cxproc.log
#strace -o cxproc.trace `pwd`/cxproc

