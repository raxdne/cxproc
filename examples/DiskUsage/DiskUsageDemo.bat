REM
REM
REM

for /f %%i in ("%0") do set CXP_CWD=%%~dpi

"%CXP_CWD%cxproc.exe" DiskUsage.cxp %CXP_CWD%

pause
