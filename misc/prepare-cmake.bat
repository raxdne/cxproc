REM
REM (p) 2020,2021,2022 A. Tenbusch
REM


REM for /f "delims=" %%i in ("%0") do set CXPBASE=%%~dpi

SET CXPBASE=%CD%\..\cxproc-build\a\b\

SET STATIC=0
SET FLAG64=1

REM VC++ Environment
IF "%FLAG64%" == "1" (
  REM CALL "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
  CALL "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) ELSE (
  CALL "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
)

IF "%STATIC%" == "1" (
  SET CXPARCH=%VSCMD_ARG_TGT_ARCH%-windows-static
) ELSE (
  SET CXPARCH=%VSCMD_ARG_TGT_ARCH%-windows
)

IF "0" == "1" (
  pushd C:\UserData\Develop\vcpkg
  REM bootstrap-vcpkg.bat -win64 -disableMetrics
  REM git pull
  .\vcpkg integrate install

  .\vcpkg --triplet %CXPARCH% --x-install-root=%CXPBASE%..\.. install curl zlib liblzma libarchive pcre2 libxml2 libxslt libexif sqlite3 duktape

  REM imagemagick libgif libjpeg libpng libtiff
  
  REM TODO: install library tools like 'sqliteshell', 'xsltproc' etc

  popd
)

set PREFIX=%CXPBASE%..\..\%CXPARCH%

SET DIR_BIN="%PREFIX%\bin"
md %DIR_BIN%
SET DIR_BUILD="%PREFIX%\build"
md %DIR_BUILD%
SET DIR_DOC="%PREFIX%\doc"
md %DIR_DOC%
REM SET DIR_TEST="%PREFIX%\test"
REM robocopy /S %CXPBASE%..\test %DIR_TEST% *.*
SET DIR_CGI="%PREFIX%\www\cgi-bin"
md %DIR_CGI%
robocopy %DIR_BIN% %DIR_CGI% *.dll

REM robocopy c:\windows\system32 %CXPBASE%..\..\%CXPARCH%\debug\bin VCRUNTIME140D.DLL
REM robocopy c:\windows\system32 %DIR_CGI% VCRUNTIME140D.DLL

SET DIR_LOG="%PREFIX%\www\log"
md %DIR_LOG%
SET DIR_WWW="%PREFIX%\www\html\test"
md %DIR_WWW%
SET DIR_CONF="%PREFIX%\www\conf"
md %DIR_CONF%

REM robocopy %CXPBASE%.. %PREFIX% *.*
REM robocopy %CXPBASE%..\doc %DIR_DOC% *.txt
REM robocopy /S %CXPBASE%..\xml %PREFIX%\xml
REM robocopy /S %CXPBASE%..\examples %PREFIX%\examples

REM robocopy /S "%PREFIX%" "C:\UserData\Develop\cxproc-build\%CXPARCH%" *.*

IF "0" == "1" (
  pushd %PREFIX%
  REM "C:\UserData\Programme\7-ZipPortable\App\7-Zip\7z.exe" a -r ..\cxproc-v1.3-pre_%CXPARCH%.zip bin xml doc examples www\cgi-bin www\html\test -x!*.pdb -x!*.ilk -x!cxproc-test.exe
  popd
)

IF "0" == "1" (
  pushd %CXPBASE%..
  REM "C:\UserData\Programme\cmake\bin\cmake.exe" -B %DIR_BUILD% -G "Visual Studio 16 2019" -A %VSCMD_ARG_TGT_CXPARCH% -DCMAKE_TOOLCHAIN_FILE=C:\UserData\Develop\vcpkg\scripts\buildsystems\vcpkg.cmake -D CXPROC_DOC:BOOL=OFF -D CXPROC_PIE:BOOL=ON -D CXPROC_EXPERIMENTAL:BOOL=OFF -D CXPROC_CXX:BOOL=OFF
  REM "C:\UserData\Programme\cmake\bin\cmake.exe" --build %DIR_BUILD% --config Release --target cxproc
  popd
)

REM TODO: CPack s. https://cmake.org/cmake/help/latest/module/CPack.html

REM pause
