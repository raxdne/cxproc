REM
REM (p) 2020,2021,2022,2026 A. Tenbusch
REM

REM for /f "delims=" %%i in ("%0") do set CXPBASE=%%~dpi

SET CXPBASE=C:\UserData\cxproc-build

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

mkdir %CXPBASE%

IF "1" == "1" (
  REM check https://learn.microsoft.com/en-us/vcpkg/
  pushd %CXPBASE%
  REM git clone https://github.com/microsoft/vcpkg.git
  cd vcpkg
  del status
  bootstrap-vcpkg.bat -disableMetrics
  git pull
  git checkout 2026.03.18
  .\vcpkg integrate install
  .\vcpkg --vcpkg-root=%CXPBASE%\vcpkg --triplet %CXPARCH% --x-install-root=%CXPBASE% --recurse install curl[tool] zlib libzip liblzma libarchive pcre2 libxml2[tools] libxslt[tools] libexif sqlite3[tool] duktape cmark[tools]
  REM imagemagick libgif libjpeg libpng libtiff
  popd
)

set PREFIX=%CXPBASE%\%CXPARCH%

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

REM robocopy c:\windows\system32 "%PREFIX%\debug\bin" VCRUNTIME140D.DLL
REM robocopy c:\windows\system32 "%DIR_CGI%" VCRUNTIME140D.DLL
FOR /D %%D IN (%PREFIX%\tools\*.*) DO robocopy "%%D" "%PREFIX%\bin" *.exe

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

IF "1" == "1" (
  pushd %DIR_BUILD%
  "%CXPBASE%\vcpkg\downloads\tools\cmake-3.31.10-windows\cmake-3.31.10-windows-x86_64\bin\cmake-gui.exe" -B . -S H:\cxproc-build\cxproc -G "Visual Studio 17 2022"
  REM "%CXPBASE%\vcpkg\downloads\tools\cmake-3.31.10-windows\cmake-3.31.10-windows-x86_64\bin\cmake.exe" -B . -S H:\cxproc-build\cxproc -A %VSCMD_ARG_TGT_CXPARCH% -G "NMake Makefiles" -DCMAKE_TOOLCHAIN_FILE=%CXPBASE%\vcpkg\scripts\buildsystems\vcpkg.cmake -D CXPROC_DOC:BOOL=OFF -D CXPROC_PIE:BOOL=ON -D CXPROC_EXPERIMENTAL:BOOL=OFF -D CXPROC_CXX:BOOL=OFF
  REM "C:\UserData\Programme\cmake\bin\cmake.exe" --build %DIR_BUILD% --config Release --target cxproc
  REM nmake /f Makefile package
  popd
)

REM pause
