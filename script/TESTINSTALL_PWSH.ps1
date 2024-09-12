# Installation path can be passed as argument
param ([string]$installpath = "$env:ProgramFiles\Environment Modules")

# Test cmd shell first
try {
   Write-Host "Testing CMD shell environment:"
   cmd /c "TESTINSTALL.bat `"$installpath`""
   if ($LASTEXITCODE -ne 0) {
      Write-Host "TESTINSTALL.bat failed with exit code $LASTEXITCODE"
      exit $LASTEXITCODE
   }
   Write-Host "CMD shell environment works!"
} catch {
   Write-Host "An error occurred while running TESTINSTALL.bat: $_"
   exit 1
}

Write-Host "`nTesting Powershell (pwsh) environment:"

# Initialize Modules if not already done
if (-not $env:MODULES_CMD) {
   . "$installpath\init\pwsh.ps1"
}

# Function to check if a command exists
function Check-Command {
   param ([string]$command)
   if (-not (Get-Command $command -ErrorAction SilentlyContinue)) {
      Write-Host "'$command' command not found"
      exit 1
   }
}

# Check commands exist
Check-Command "envmodule"
Check-Command "ml"

# Function to run a command and check its output
function Run-Command {
   param (
      [string]$command,
      [string]$expectedOutput
   )
   if ((Invoke-Expression $command 2>&1) -notmatch $expectedOutput) {
      Write-Host "'$command' test failed"
      exit 1
   }
}

Run-Command "envmodule --version" "Modules Release"
Run-Command "envmodule list" "No Modulefiles Currently Loaded."
Run-Command "ml" "No Modulefiles Currently Loaded."

# Load null module
Invoke-Expression "envmodule load null"

Run-Command "envmodule list -t" "null"
Run-Command "ml -t" "null"

# Unload null module
Invoke-Expression "ml -null"

Run-Command "envmodule list" "No Modulefiles Currently Loaded."
Run-Command "ml" "No Modulefiles Currently Loaded."

Write-Host "Powershell (pwsh) environment works!"
