
# script to analyze include dependencies, when running 'gcc ... -M -MM -H'

{
    if (/^\.+.*/ && /cxproc/ && ! (/c-dt/ || /res_/)) {
	print;
    } 
    else if (/-c /) {
	print "\n", $NF;
    }
    else {
    }
}

