@echo off

:: installation path can be passed as argument
if [%1]==[] (
   set "installpath=%ProgramFiles%\Environment Modules"
) else (
   set "installpath=%1"
)

:: remove bin directory from system path
set "binpath=%installpath%\bin"
setlocal enableextensions enabledelayedexpansion
set "NEWPATH=!PATH:%binpath%;=!"
if not "%NEWPATH%" == "%PATH%" (
   set "PATH=%NEWPATH%"
   setx /M PATH "%NEWPATH%"
)
if errorlevel 1 ( exit /b 1 )

:: remove installation directory and content
rd /s /q "%installpath%"
:: quit on error if directory still exists
if exist "%installpath%" ( exit /b 2 )

:: vim:set tabstop=3 shiftwidth=3 expandtab autoindent:
