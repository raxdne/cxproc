REM
REM
REM

for /f "delims=" %%i in ("%0") do set CXP_PATH=%%~dpi..//

rmdir /Q /S tmp
mkdir tmp

REM "C:\UserData\Programme\cmake\bin\ctest.exe" -C Debug
"C:\UserData\Programme\cmake\bin\ctest.exe" -C Debug -R "^cgi-"

REM bin\cxproc.exe test\config-test.cxp tmp

pause