@echo off
set OUTPUT=
set MODCMD="call tclsh %MODULESHOME%\modulecmd.tcl cmd %*"
REM -- Each output line needs to be executed in cmd shell
for /f "tokens=*" %%I in ('%MODCMD%') do %%I
