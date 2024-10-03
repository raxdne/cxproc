
# Testing cxproc

## cxproc-test

	cxproc-test -t basics

## filex

	../x86_64-gnu-linux/bin/filex README.md > test.pie

## dir2csv

	../x86_64-gnu-linux/bin/dir2csv . > test.csv

## dir2sqlite

	../x86_64-gnu-linux/bin/dir2sqlite . | sqlite3 test.db3

## cxproc

	../x86_64-gnu-linux/bin/cxproc -e

## cxproc-cgi

[CRUD](https://en.wikipedia.org/wiki/Create,_read,_update_and_delete)

__Create__ new text file

	curl 'http://localhost:8183/cxproc/exe?path=TEST-1.txt&cxp=SaveContentXml&strContent=*+TEST+1'

__Read__ text file

	curl 'http://localhost:8183/TEST-1.txt'

__Update__ existing text file

	curl 'http://localhost:8183/cxproc/exe?path=TEST-1.txt&cxp=SaveContentXml&strContent=*+TEST+12'
	curl 'http://localhost:8183/cxproc/exe?path=TEST-1.txt&cxp=PiejQEditor'

__Delete__ existing text file (move to default Trash folder)

	curl 'http://localhost:8183/cxproc/exe?path=TEST-1.txt&cxp=MoveFileTo'
	curl 'http://localhost:8183/cxproc/exe?path=TEST-1.txt&cxp=MoveFileTo&to=xyz'

TODO: Upload existing text file

## CTest

	(cd $DIR_BUILD && ctest)
