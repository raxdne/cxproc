REM
REM (p) 2016 A. Tenbusch
REM

SET STATIC=0
SET DEBUG=1
SET FLAG64=0

for /f "delims=" %%i in ("%0") do set CXPBASE=%%~dpi

set PREFIX=%CXPBASE%..\..\

set PREFIX=%PREFIX%x86
IF "%FLAG64%" == "1" (
  set PREFIX=%PREFIX%_64
)
set PREFIX=%PREFIX%-windows

IF "%DEBUG%" == "1" (
  set PREFIX=%PREFIX%-debug
) ELSE (
  set PREFIX=%PREFIX%-release
)

IF "%STATIC%" == "1" (
  set PREFIX=%PREFIX%-static
) ELSE (
  set PREFIX=%PREFIX%-dynamic
)

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
SET DIR_TEST="%PREFIX%\test"
robocopy /S %CXPBASE%..\test %DIR_TEST% *.*
SET DIR_CGI="%PREFIX%\www\cgi-bin"
md %DIR_CGI%
robocopy %DIR_BIN% %DIR_CGI% *.dll
SET DIR_LOG="%PREFIX%\www\log"
md %DIR_LOG%
SET DIR_WWW="%PREFIX%\www\html"
md %DIR_WWW%

IF EXIST "%DIR_WWW%\Documents" (
)

