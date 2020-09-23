REM
REM
REM

for /f %%i in ("%0") do set TESTDIR=%%~dpi

md %TESTDIR%..\..\tmp

set CXP_BACKUPS=Backup/
set CXP_LOG=4
set CXP_LOGFILE=%TESTDIR%..\..\tmp\test-cgi.log
set CXP_ROOT=C:\User\develop\cxproc-build-vc\www
set CXP_TEMPLATES=www/Templates/
set CXP_PATH=%TESTDIR%..\..\\

REM set PATH="%TESTDIR%..\..\bin";%PATH%

set DOCUMENT_ROOT="C:\User\develop\cxproc-build-vc\www"

set GATEWAY_INTERFACE="CGI/1.1"

set HTTP_ACCEPT_CHARSET="ISO-8859-1,utf-8;q=0.7,*;q=0.7"
set HTTP_ACCEPT_ENCODING="gzip,deflate"
set HTTP_ACCEPT_LANGUAGE="de-de,de;q=0.8,en-us;q=0.5,en;q=0.3"
set HTTP_ACCEPT="text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
set HTTP_CACHE_CONTROL="max-age=0"
set HTTP_CONNECTION="keep-alive"
set HTTP_HOST="localhost:8080"
set HTTP_KEEP_ALIVE="300"
set HTTP_USER_AGENT="Mozilla/5.0 (Windows; U; Windows NT 5.1; de; rv:1.9.1.7) Gecko/20091221 Firefox/3.5.7 (.NET CLR 3.5.30729)"

set QUERY_STRING=file=
set REMOTE_ADDR="127.0.0.1"
set REMOTE_PORT="1071"
set REQUEST_METHOD=GET
set REQUEST_URI="/cxproc/exe?file=Test.txt"
set SCRIPT_FILENAME="%TESTDIR%../../bin/cxproc.exe"
set SCRIPT_NAME="/cxproc/exe"
set SERVER_ADDR="127.0.0.1"
set SERVER_ADMIN="root@localhost"
set SERVER_NAME="localhost"
set SERVER_PORT="8080"
set SERVER_PROTOCOL="HTTP/1.1"
set SERVER_SIGNATURE=""
set SERVER_SOFTWARE="Apache/2.2.14 (Win32)"

"%TESTDIR%..\..\bin\cxproc.exe" > %TESTDIR%..\..\tmp\test-cgi-out.log 2> %TESTDIR%..\..\tmp\test-cgi-err.log

REM pause
