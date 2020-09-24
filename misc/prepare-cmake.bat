REM
REM (p) 2016 A. Tenbusch
REM

IF "%VSINSTALLDIR%"=="" (
  CALL ..\srclib\env-vc.bat
)

md %DIR_BUILD%
md %DIR_BIN%
md %DIR_LIB%
md %DIR_INC%
md %DIR_DOC%
IF EXIST %DIR_TEST% (
  svn update %DIR_TEST%
) ELSE (
  svn co http://arrakis.fritz.box:8187/cxproc/trunk/test %DIR_TEST%
)
md %DIR_CGI%
robocopy %DIR_BIN% %DIR_CGI% *.dll
REM md %DIR_HTTP%
REM robocopy %DIR_BIN% %DIR_HTTP% *.dll
md %DIR_LOG%
md %DIR_WWW%

IF EXIST "%DIR_WWW%\pie" (
  svn update "%DIR_WWW%\pie"
) ELSE (
  REM md %DIR_WWW%\pie
  svn co http://arrakis.fritz.box:8187/www/html/trunk "%DIR_WWW%\pie"
)

IF EXIST "%DIR_WWW%\Documents" (
  svn update "%DIR_WWW%\Documents"
) ELSE (
  svn co http://arrakis.fritz.box:8187/cxproc/trunk/contrib/pie/www "%DIR_WWW%\Documents"
)

REM robocopy %DIR_WWW%\pie\browser-cgi %DIR_WWW%\Documents index.html
