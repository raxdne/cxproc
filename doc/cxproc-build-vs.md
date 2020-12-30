
# [Cxproc](https://github.com/raxdne/cxproc) Build Environment on MS Windows

## [Visual Studio Community 2019](https://visualstudio.microsoft.com/de/downloads/) C/C++

English GUI or
[Language Pack](https://docs.microsoft.com/de-de/visualstudio/install/install-visual-studio?view=vs-2019#step-6---install-language-packs-optional)

- "Tools > Get Tools and Features"
- "Tools > Options > Environment > International Settings > Language"

## [vcpkg](https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019): a C++ package manager for Windows, Linux, and MacOS

> Visual Studio. VS2015 or VS2017 (with C++) needs to be installed. All libraries in the vcpkg Windows catalog have been tested for compatibility with Visual Studio 2015, Visual Studio 2017, and Visual Studio 2019.

s. `vcpkg\README.md` and `vcpkg\ports`

__English__ Build Environment required (s. above)

    git clone https://github.com/microsoft/vcpkg.git
    
    .\cxproc\misc\prepare-cmake.bat
	
### Microsoft Visual Studio Runtime

Debug Runtime (Redistribution **is not** permitted)

    robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\debug_nonredist\x86\Microsoft.VC142.DebugCRT" "%CXP_TARGET%\bin" vcruntime140d.dll
    robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\debug_nonredist\x86\Microsoft.VC142.DebugCRT" "%CXP_TARGET%\www\cgi-bin" vcruntime140d.dll
    robocopy "C:\Windows\SysWOW64" "%CXP_TARGET%\bin" ucrtbased.dll
    robocopy "C:\Windows\SysWOW64" "%CXP_TARGET%\www\cgi-bin" ucrtbased.dll


Release Runtime (s. `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\v142`)

    robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\onecore\x86\Microsoft.VC142.CRT" "C:\UserData\Develop\cxproc-build\trunk\x86-windows-dynamic\bin" vcruntime140.dll
    robocopy "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Redist\MSVC\14.26.28720\onecore\x86\Microsoft.VC142.CRT" "%CXP_TARGET%\www\cgi-bin" vcruntime140.dll

## Apache 2.4

Setup

- [ApacheHaus](https://www.apachehaus.com/cgi-bin/download.plx)
- s. Template in `cxproc/www/conf/httpd`

### Thirdparty Software for Web Usage


## [Notepad++](https://notepad-plus-plus.org/downloads/)

XML Tools

## [Dependencies](https://github.com/lucasg/Dependencies/releases)

## [Git for Windows](https://gitforwindows.org/)

## [PortableGit](https://sourceforge.net/projects/gitportable/)

`vcpkg\downloads\tools\git-2.26.2-1-windows`

## TortoiseGit

## [CMake](https://cmake.org/download/)

s. `vcpkg\downloads`

	SET PATH=%PATH%;C:\User\Programme\cmake\bin

## CTest

	"C:\UserData\Programme\cmake\bin\ctest.exe" -C Debug

## [Firefox](https://www.mozilla.org/en-US/firefox/all/#product-desktop-release)

## [HxD](https://mh-nexus.de/de/hxd/)
