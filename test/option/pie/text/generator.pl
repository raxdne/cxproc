#
#
#

open FILE, "> test-pie-2.txt" or die "ERROR";

print FILE "\n* Section";
for (1..5) {
    print FILE "\n\n** Section\n\n";
    print FILE "ABC " x 128;
}

close FILE;


open FILE, "> test-pie-3.txt" or die "ERROR";

for $i (1..8) {
    for $j (1..8) {
	print FILE '*' x $j . " Par $i.$j\r\n\r\n";
    }

    for (1..5) {
	for (1..50) {
	    print FILE 'A' x (128) . " ";
	}
	print FILE "\n\r\n";
    }
}

close FILE;
