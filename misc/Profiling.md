
# Profiling

	CXP_PATH=~/cxproc-build/x86_64-gnu-linux/www/html/pie// time ~/cxproc-build/x86_64-gnu-linux/bin/cxproc t-2.cxp
	
	gprof ~/cxproc-build/x86_64-gnu-linux/bin/cxproc | gprof2dot -n0 -e0 | dot -Tpng -o output.png
