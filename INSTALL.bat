@echo off

:: installation path can be passed as argument
if [%1]==[] (
   set "installpath=%ProgramFiles%\Environment Modules"
) else (
   set "installpath=%1"
)

:: create installation directory
md "%installpath%"
:: quit if directory not created
if not exist "%installpath%" ( exit /b 1 )

:: install files
xcopy /Y /S /F "%~dp0\*" "%installpath%\"
:: quit on error
if errorlevel 1 ( exit /b 2 )

:: add bin directory to system path if not done yet
set "binpath=%installpath%\bin"
echo %PATH% | find /i "%binpath:"=%">nul || setx /M PATH "%binpath%;%PATH%"
if errorlevel 1 ( exit /b 3 )

:: vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
