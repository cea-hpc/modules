@echo off
:: Initialize Modules if already done yet
if not defined MODULESHOME goto:initModules
if not defined MODULES_CMD goto:initModules
if "%MODULESHOME%\bin\module.cmd" NEQ "%~dpnx0" goto:initModules
goto:skipInit

:initModules
call "%~dp0moduleinit.cmd" %*

:skipInit
set OUTPUT=
set MODCMD="call tclsh %MODULES_CMD% cmd %*"
REM -- Each output line needs to be executed in cmd shell
for /f "tokens=*" %%I in ('%MODCMD%') do %%I
goto:Done

:reportError
echo Modules is not initialized yet. Please call init\profile.cmd from your
echo Modules installation directory before to use module command.

:Done
