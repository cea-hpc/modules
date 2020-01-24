@echo off

set FIND=%SYSTEMROOT%\system32\find

:: check commands exist
where /Q module.cmd
if errorlevel 1 (
    echo "'module' command not found"
    exit /b 1
)
where /Q ml.cmd
if errorlevel 1 (
    echo "'module' command not found"
    exit /b 1
)

module -V 2>&1 | %FIND% "Modules Release"
if errorlevel 1 (
    echo "'module -V' test failed"
    exit /b 1
)
module list 2>&1 | %FIND% "No Modulefiles Currently Loaded."
if errorlevel 1 (
    echo "'module list' test failed"
    exit /b 1
)
ml 2>&1 | %FIND% "No Modulefiles Currently Loaded."
if errorlevel 1 (
    echo "'ml' test failed"
    exit /b 1
)

call module load null

module list -t 2>&1 | %FIND% "null"
if errorlevel 1 (
    echo "'module list -t' test failed"
    exit /b 1
)
ml -t 2>&1 | %FIND% "null"
if errorlevel 1 (
    echo "'ml -t' test failed"
    exit /b 1
)

call ml -null

module list 2>&1 | %FIND% "No Modulefiles Currently Loaded."
if errorlevel 1 (
    echo "'module list' test failed"
    exit /b 1
)
ml 2>&1 | %FIND% "No Modulefiles Currently Loaded."
if errorlevel 1 (
    echo "'ml' test failed"
    exit /b 1
)

