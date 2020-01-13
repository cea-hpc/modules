@echo off
REM -----------------------------------------------
REM Script to customize module-tcl for "cmd" shell
REM Load the modulepaths with "module use"
REM -----------------------------------------------

:: Define MODULESHOME is defined yet
if defined MODULESHOME goto:skipInit
if not exist "%~dp0..\libexec\modulecmd.tcl" goto:skipInit
for %%i in ("%~dp0..") do set "MODULESHOME=%%~dpni"

:skipInit

:: check if module.cmd directory needs to be added to PATH
if not exist "%~dp0..\bin\module.cmd" goto:skipModuleInit
for %%i in (module.cmd) do ( 
    if "x%%~$PATH:i" EQU "x" (
        for %%a in (%~dp0.) do set "PATH=%%~dpnxa;%PATH%"
    )
)

:skipModuleInit

:: check if tclsh.exe directory must be added to PATH
if not exist "%~dp0..\bin\tclsh.exe" goto:skipTclShInit
for %%i in (tclsh) do ( 
    if "x%%~$PATH:i" EQU "x" (
        for %%a in (%~dp0.) do set "PATH=%%~dpnxa;%PATH%"
    )
)

:skipTclShInit

if not defined MODULES_CMD set MODULES_CMD=%~dp0..\libexec\modulecmd.tcl
call module use %MODULESHOME%\modulefiles
:: Display available modules only if no input argument
if "%~1" EQU "" call module avail
