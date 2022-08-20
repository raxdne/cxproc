#
#
#

DIR_PREFIX=/home/developer/cxproc-build/x86_64-gnu-linux/

export CXP_PATH="$DIR_PREFIX/www/html/pie//"
export CXP_BACKUPS="Backup/"
export CXP_TEMPLATES="Templates/"
export CXP_LOG="4"
#export CXP_DATE="2010"

TMP=$DIR_PREFIX/tmp

test -d $TMP || mkdir $TMP

export CXP_LOGFILE="$TMP/cxproc-cli.log"

#$DIR_PREFIX/bin/cxproc 1> $TMP/cxproc-cli.txt 2> $TMP/cxproc-cli.err

valgrind --leak-check=full --show-error-list=yes --show-reachable=yes --show-leak-kinds=all --log-file=$TMP/cxproc-cli-valgrind.log $DIR_PREFIX/bin/cxproc $DIR_PREFIX/www/html/develop/t-11-a.cxp 1> $TMP/cxproc-cli.txt 2> $TMP/cxproc-cli.err

#valgrind --tool=callgrind --callgrind-out-file=$TMP/cxproc-cli-callgrind.log --log-file=$TMP/cxproc-cli-valgrind.log $DIR_PREFIX/bin/cxproc 1> $TMP/cxproc-cli.txt 2> $TMP/cxproc-cli.err

#valgrind --tool=cachegrind --cachegrind-out-file=$TMP/cxproc-cli-cachegrind.log --log-file=$TMP/cxproc-cli-valgrind.log $DIR_PREFIX/bin/cxproc 1> $TMP/cxproc-cli.txt 2> $TMP/cxproc-cli.err

# s. http://code.google.com/p/jrfonseca/wiki/Gprof2Dot
#gprof ./cxproc ~/www/gmon.out | ~/Desktop/gprof2dot.py | dot -Tpng -o output.png
#gprof -b ./cxproc ~/www/gmon.out &> $TMP/cxproc-cli-profile.log

#strace -o cxproc.trace `pwd`/cxproc

