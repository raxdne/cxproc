
# [Cxproc](http://www.tenbusch.info/) Build Environment

## [Visual Studio Community 2019](https://visualstudio.microsoft.com/de/downloads/) C/C++

English GUI or
[Language Pack](https://docs.microsoft.com/de-de/visualstudio/install/install-visual-studio?view=vs-2019#step-6---install-language-packs-optional)

- "Tools > Get Tools and Features"
- "Tools > Options > Environment > International Settings > Language"

https://tortoisesvn.net/visualstudio.html

## [Visual Studio Code](https://github.com/microsoft/vscode)

Extensions
- "CMake Tools" (Microsoft)
- "CMake Integration"
- "CMake Tools"
- "cmake-format"
- Subversion "SVN"
- Markdown "Markdown All In One"
- C/C++ Integration (Debugger etc)

## [cloc](https://github.com/AlDanial/cloc)


## CMake

``vcpkg\downloads``

```
SET PATH=%PATH%;C:\User\Programme\cmake\bin
```

## Subversion & [TortoiseSVN](https://tortoisesvn.net/)

```
SET PATH=%PATH%;C:\Program Files\TortoiseSVN\bin

svn co http://arrakis.fritz.box:8187/cxproc/trunk C:\UserData\Develop\cxproc-build
cd C:\UserData\Develop\cxproc-build\trunk\srclib
env-vc.bat
cd C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic
```

## [PortableGit](https://sourceforge.net/projects/gitportable/)

``vcpkg\downloads\tools\git-2.26.2-1-windows``

## Notepad++

Plugins
- markdown
- XML

## [vcpkg](https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019): a C++ package manager for Windows, Linux, and MacOS

> Visual Studio. VS2015 or VS2017 (with C++) needs to be installed. All libraries in the vcpkg Windows catalog have been tested for compatibility with Visual Studio 2015, Visual Studio 2017, and Visual Studio 2019.

s. "vcpkg\README.md" and "vcpkg\ports"

__English__ Build Environment required (s. above)

``` BAT file
git clone https://github.com/microsoft/vcpkg.git
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

bootstrap-vcpkg.bat
.\vcpkg integrate install
.\vcpkg install curl
.\vcpkg install pcre2
.\vcpkg install libarchive
.\vcpkg install libxslt
.\vcpkg install libxslt
.\vcpkg install sqlite3
.\vcpkg install duktape

REM imagemagick libgif libexif libjpeg libpng libtiff

robocopy /S installed\x86-windows\debug\bin C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\bin *.*
robocopy /S installed\x86-windows\debug\bin C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\www\cgi-bin *.*
robocopy /S installed\x86-windows\debug\lib C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\lib *.*
robocopy /S installed\x86-windows\include C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\include *.*

robocopy /S installed\x86-windows\bin C:\UserData\Develop\cxproc-build\trunk\x86-windows-dynamic\bin *.*
robocopy /S installed\x86-windows\bin C:\UserData\Develop\cxproc-build\trunk\x86-windows-dynamic\www\cgi-bin *.*
robocopy /S installed\x86-windows\lib C:\UserData\Develop\cxproc-build\trunk\x86-windows-dynamic\lib *.*
robocopy /S installed\x86-windows\include C:\UserData\Develop\cxproc-build\trunk\x86-windows-dynamic\include *.*
```

### Microsoft Visual Studio Runtime

Debug Runtime (Redistribution **is not** permitted)
```
robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\debug_nonredist\x86\Microsoft.VC142.DebugCRT" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\bin" vcruntime140d.dll
robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\debug_nonredist\x86\Microsoft.VC142.DebugCRT" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\www\cgi-bin" vcruntime140d.dll
robocopy "C:\Windows\SysWOW64" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\bin" ucrtbased.dll
robocopy "C:\Windows\SysWOW64" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\www\cgi-bin" ucrtbased.dll
```

Release Runtime (s. ``C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\v142``)
```
robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\onecore\x86\Microsoft.VC142.CRT" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-dynamic\bin" vcruntime140.dll
robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\onecore\x86\Microsoft.VC142.CRT" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-debug-dynamic\www\cgi-bin" vcruntime140.dll
```

## Windows Subsystem for Linux [WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10)

## Hyper-V

Setup
- Windows Features
- Shared folder (smbclient on Linux)
- Graphics
- Clipboard

## IIS 10

Setup
- Windows Features
- Configuration generator s. "cxproc-build\trunk\contrib\pie\IIS"

## Apache 2.4

Setup
- [ApacheHaus](https://www.apachehaus.com/cgi-bin/download.plx)
- s. Template in "cxproc-build\trunk\contrib\pie\httpd"

### Thirdparty Software for Web Usage


## Dependency Walker x86/x64_86

## Sqliteman

## WinMerge

## CTest

```
"C:\UserData\Programme\cmake\bin\ctest.exe" -C Debug
```

