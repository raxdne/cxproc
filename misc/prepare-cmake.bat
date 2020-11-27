REM
REM (p) 2020 A. Tenbusch
REM

for /f "delims=" %%i in ("%0") do set CXPBASE=%%~dpi

SET STATIC=0
SET FLAG64=1

REM VC++ Environment
IF "%FLAG64%" == "1" (
  CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
) ELSE (
  CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
)

SET ARCH=%VSCMD_ARG_TGT_ARCH%-windows-static

pushd C:\UserData\Develop\vcpkg
REM bootstrap-vcpkg.bat -win64 -disableMetrics
.\vcpkg integrate install

.\vcpkg --triplet %ARCH% --x-install-root=%CXPBASE%..\.. install zlib liblzma pcre2 libxml2 libxslt libexif sqlite3 duktape

REM curl libarchive imagemagick libgif libjpeg libpng libtiff
popd

set PREFIX=%CXPBASE%..\..\%ARCH%

SET DIR_BUILD="%PREFIX%\build"
md %DIR_BUILD%
SET DIR_DOC="%PREFIX%\doc"
md %DIR_DOC%
REM SET DIR_TEST="%PREFIX%\test"
REM robocopy /S %CXPBASE%..\test %DIR_TEST% *.*
SET DIR_CGI="%PREFIX%\www\cgi-bin"
md %DIR_CGI%
robocopy %DIR_BIN% %DIR_CGI% *.dll

REM robocopy c:\windows\system32 %CXPBASE%..\..\%ARCH%\debug\bin VCRUNTIME140D.DLL
REM robocopy c:\windows\system32 %DIR_CGI% VCRUNTIME140D.DLL

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
REM "C:\UserData\Programme\7-ZipPortable\App\7-Zip\7z.exe" a -r ..\cxproc-v1.3-pre_%ARCH%.zip bin xml doc examples www\cgi-bin www\html\test -x!*.pdb -x!*.ilk -x!cxproc-test.exe
popd

pushd %CXPBASE%..
REM "C:\UserData\Programme\cmake\bin\cmake.exe" -B %DIR_BUILD% -G "Visual Studio 16 2019" -A %VSCMD_ARG_TGT_ARCH% -DCMAKE_TOOLCHAIN_FILE=C:\UserData\Develop\vcpkg\scripts\buildsystems\vcpkg.cmake -D CXPROC_DOC:BOOL=OFF -D CXPROC_PIE:BOOL=ON -D CXPROC_EXPERIMENTAL:BOOL=OFF -D CXPROC_DEBUG:BOOL=OFF -D CXPROC_CXX:BOOL=OFF
popd

REM TODO: CPack s. https://cmake.org/cmake/help/latest/module/CPack.html

REM pause
