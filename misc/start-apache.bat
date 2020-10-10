@echo off
REM
REM
REM

REM Start "HTTPD" /b "C:\User\Programme\Apache24\bin\httpd.exe" -w -f "C:\User\develop\cxproc-build\trunk\x86-windows-debug-dynamic\www\etc\Apache24\httpd.conf"
Start "HTTPD" /b "C:\UserData\Programme\Apache24\bin\httpd.exe" -w

ECHO "Apache HTTPD"
ECHO "  Default :80"
ECHO "     Test :8182"
ECHO "  Develop :8187"

explorer.exe http://localhost:8187
