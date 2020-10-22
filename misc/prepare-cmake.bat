REM
REM (p) 2020 A. Tenbusch
REM

for /f "delims=" %%i in ("%0") do set CXPBASE=%%~dpi

SET STATIC=0
SET DEBUG=1
SET FLAG64=0

set ARCH=x86
IF "%FLAG64%" == "1" (
  set ARCH=%ARCH%_64
)
set ARCH=%ARCH%-windows

IF "%DEBUG%" == "1" (
  set ARCH=%ARCH%-debug
)

IF "%STATIC%" == "1" (
  set ARCH=%ARCH%-static
)

set PREFIX=%CXPBASE%..\..\%ARCH%

SET DIR_BUILD="%PREFIX%\build"
md %DIR_BUILD%
SET DIR_BIN="%PREFIX%\bin"
md %DIR_BIN%
SET DIR_LIB="%PREFIX%\lib"
md %DIR_LIB%
SET DIR_INC="%PREFIX%\include"
md %DIR_INC%
SET DIR_DOC="%PREFIX%\doc"
md %DIR_DOC%
REM SET DIR_TEST="%PREFIX%\test"
REM robocopy /S %CXPBASE%..\test %DIR_TEST% *.*
SET DIR_CGI="%PREFIX%\www\cgi-bin"
md %DIR_CGI%
robocopy %DIR_BIN% %DIR_CGI% *.dll
SET DIR_LOG="%PREFIX%\www\log"
md %DIR_LOG%
SET DIR_WWW="%PREFIX%\www\html"
md %DIR_WWW%
SET DIR_CONF="%PREFIX%\www\conf"
md %DIR_CONF%

robocopy %CXPBASE%.. %PREFIX% *.*
robocopy %CXPBASE%..\doc %DIR_DOC% *.txt
robocopy /S %CXPBASE%..\xml %PREFIX%\xml
robocopy /S %CXPBASE%..\examples %PREFIX%\examples

pushd %PREFIX%
"C:\UserData\Programme\7-ZipPortable\App\7-Zip\7z.exe" a -r ..\cxproc-v1.3-pre_%ARCH%.zip bin xml doc examples www\cgi-bin www\html\test -x!*.pdb -x!*.ilk -x!cxproc-test.exe
popd

REM TODO: CPack s. https://cmake.org/cmake/help/latest/module/CPack.html
