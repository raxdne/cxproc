#
#
#

file ~/cxproc-build/x86_64-gnu-linux/bin/cxproc

run -t ~/cxproc-build/cxproc/test/option/pie/text/csv/test-pie-import-csv-001.csv

break cxpCtxtCliParse

