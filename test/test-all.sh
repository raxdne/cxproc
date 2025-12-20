#
#
#

for D in calendar dir each plain system xhtml xml xsl
do
    (cd $D && mkdir result && valgrind --tool=memcheck --leak-check=full --fullpath-after= --num-callers=20 ~/cxproc-build/x86_64-gnu-linux/bin/cxproc config.cxp 2> result/test-$D.log)
done

