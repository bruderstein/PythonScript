<#
.Synopsis
Activate a Python virtual environment for the current PowerShell session.

.Description
Pushes the python executable for a virtual environment to the front of the
$Env:PATH environment variable and sets the prompt to signify that you are
in a Python virtual environment. Makes use of the command line switches as
well as the `pyvenv.cfg` file values present in the virtual environment.

.Parameter VenvDir
Path to the directory that contains the virtual environment to activate. The
default value for this is the parent of the directory that the Activate.ps1
script is located within.

.Parameter Prompt
The prompt prefix to display when this virtual environment is activated. By
default, this prompt is the name of the virtual environment folder (VenvDir)
surrounded by parentheses and followed by a single space (ie. '(.venv) ').

.Example
Activate.ps1
Activates the Python virtual environment that contains the Activate.ps1 script.

.Example
Activate.ps1 -Verbose
Activates the Python virtual environment that contains the Activate.ps1 script,
and shows extra information about the activation as it executes.

.Example
Activate.ps1 -VenvDir C:\Users\MyUser\Common\.venv
Activates the Python virtual environment located in the specified location.

.Example
Activate.ps1 -Prompt "MyPython"
Activates the Python virtual environment that contains the Activate.ps1 script,
and prefixes the current prompt with the specified string (surrounded in
parentheses) while the virtual environment is active.

.Notes
On Windows, it may be required to enable this Activate.ps1 script by setting the
execution policy for the user. You can do this by issuing the following PowerShell
command:

PS C:\> Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

For more information on Execution Policies: 
https://go.microsoft.com/fwlink/?LinkID=135170

#>
Param(
    [Parameter(Mandatory = $false)]
    [String]
    $VenvDir,
    [Parameter(Mandatory = $false)]
    [String]
    $Prompt
)

<# Function declarations --------------------------------------------------- #>

<#
.Synopsis
Remove all shell session elements added by the Activate script, including the
addition of the virtual environment's Python executable from the beginning of
the PATH variable.

.Parameter NonDestructive
If present, do not remove this function from the global namespace for the
session.

#>
function global:deactivate ([switch]$NonDestructive) {
    # Revert to original values

    # The prior prompt:
    if (Test-Path -Path Function:_OLD_VIRTUAL_PROMPT) {
        Copy-Item -Path Function:_OLD_VIRTUAL_PROMPT -Destination Function:prompt
        Remove-Item -Path Function:_OLD_VIRTUAL_PROMPT
    }

    # The prior PYTHONHOME:
    if (Test-Path -Path Env:_OLD_VIRTUAL_PYTHONHOME) {
        Copy-Item -Path Env:_OLD_VIRTUAL_PYTHONHOME -Destination Env:PYTHONHOME
        Remove-Item -Path Env:_OLD_VIRTUAL_PYTHONHOME
    }

    # The prior PATH:
    if (Test-Path -Path Env:_OLD_VIRTUAL_PATH) {
        Copy-Item -Path Env:_OLD_VIRTUAL_PATH -Destination Env:PATH
        Remove-Item -Path Env:_OLD_VIRTUAL_PATH
    }

    # Just remove the VIRTUAL_ENV altogether:
    if (Test-Path -Path Env:VIRTUAL_ENV) {
        Remove-Item -Path env:VIRTUAL_ENV
    }

    # Just remove VIRTUAL_ENV_PROMPT altogether.
    if (Test-Path -Path Env:VIRTUAL_ENV_PROMPT) {
        Remove-Item -Path env:VIRTUAL_ENV_PROMPT
    }

    # Just remove the _PYTHON_VENV_PROMPT_PREFIX altogether:
    if (Get-Variable -Name "_PYTHON_VENV_PROMPT_PREFIX" -ErrorAction SilentlyContinue) {
        Remove-Variable -Name _PYTHON_VENV_PROMPT_PREFIX -Scope Global -Force
    }

    # Leave deactivate function in the global namespace if requested:
    if (-not $NonDestructive) {
        Remove-Item -Path function:deactivate
    }
}

<#
.Description
Get-PyVenvConfig parses the values from the pyvenv.cfg file located in the
given folder, and returns them in a map.

For each line in the pyvenv.cfg file, if that line can be parsed into exactly
two strings separated by `=` (with any amount of whitespace surrounding the =)
then it is considered a `key = value` line. The left hand string is the key,
the right hand is the value.

If the value starts with a `'` or a `"` then the first and last character is
stripped from the value before being captured.

.Parameter ConfigDir
Path to the directory that contains the `pyvenv.cfg` file.
#>
function Get-PyVenvConfig(
    [String]
    $ConfigDir
) {
    Write-Verbose "Given ConfigDir=$ConfigDir, obtain values in pyvenv.cfg"

    # Ensure the file exists, and issue a warning if it doesn't (but still allow the function to continue).
    $pyvenvConfigPath = Join-Path -Resolve -Path $ConfigDir -ChildPath 'pyvenv.cfg' -ErrorAction Continue

    # An empty map will be returned if no config file is found.
    $pyvenvConfig = @{ }

    if ($pyvenvConfigPath) {

        Write-Verbose "File exists, parse `key = value` lines"
        $pyvenvConfigContent = Get-Content -Path $pyvenvConfigPath

        $pyvenvConfigContent | ForEach-Object {
            $keyval = $PSItem -split "\s*=\s*", 2
            if ($keyval[0] -and $keyval[1]) {
                $val = $keyval[1]

                # Remove extraneous quotations around a string value.
                if ("'""".Contains($val.Substring(0, 1))) {
                    $val = $val.Substring(1, $val.Length - 2)
                }

                $pyvenvConfig[$keyval[0]] = $val
                Write-Verbose "Adding Key: '$($keyval[0])'='$val'"
            }
        }
    }
    return $pyvenvConfig
}


<# Begin Activate script --------------------------------------------------- #>

# Determine the containing directory of this script
$VenvExecPath = Split-Path -Parent $MyInvocation.MyCommand.Definition
$VenvExecDir = Get-Item -Path $VenvExecPath

Write-Verbose "Activation script is located in path: '$VenvExecPath'"
Write-Verbose "VenvExecDir Fullname: '$($VenvExecDir.FullName)"
Write-Verbose "VenvExecDir Name: '$($VenvExecDir.Name)"

# Set values required in priority: CmdLine, ConfigFile, Default
# First, get the location of the virtual environment, it might not be
# VenvExecDir if specified on the command line.
if ($VenvDir) {
    Write-Verbose "VenvDir given as parameter, using '$VenvDir' to determine values"
}
else {
    Write-Verbose "VenvDir not given as a parameter, using parent directory name as VenvDir."
    $VenvDir = $VenvExecDir.Parent.FullName.TrimEnd("\\/")
    Write-Verbose "VenvDir=$VenvDir"
}

# Next, read the `pyvenv.cfg` file to determine any required value such
# as `prompt`.
$pyvenvCfg = Get-PyVenvConfig -ConfigDir $VenvDir

# Next, set the prompt from the command line, or the config file, or
# just use the name of the virtual environment folder.
if ($Prompt) {
    Write-Verbose "Prompt specified as argument, using '$Prompt'"
}
else {
    Write-Verbose "Prompt not specified as argument to script, checking pyvenv.cfg value"
    if ($pyvenvCfg -and $pyvenvCfg['prompt']) {
        Write-Verbose "  Setting based on value in pyvenv.cfg='$($pyvenvCfg['prompt'])'"
        $Prompt = $pyvenvCfg['prompt'];
    }
    else {
        Write-Verbose "  Setting prompt based on parent's directory's name. (Is the directory name passed to venv module when creating the virtual environment)"
        Write-Verbose "  Got leaf-name of $VenvDir='$(Split-Path -Path $venvDir -Leaf)'"
        $Prompt = Split-Path -Path $venvDir -Leaf
    }
}

Write-Verbose "Prompt = '$Prompt'"
Write-Verbose "VenvDir='$VenvDir'"

# Deactivate any currently active virtual environment, but leave the
# deactivate function in place.
deactivate -nondestructive

# Now set the environment variable VIRTUAL_ENV, used by many tools to determine
# that there is an activated venv.
$env:VIRTUAL_ENV = $VenvDir

$env:VIRTUAL_ENV_PROMPT = $Prompt

if (-not $Env:VIRTUAL_ENV_DISABLE_PROMPT) {

    Write-Verbose "Setting prompt to '$Prompt'"

    # Set the prompt to include the env name
    # Make sure _OLD_VIRTUAL_PROMPT is global
    function global:_OLD_VIRTUAL_PROMPT { "" }
    Copy-Item -Path function:prompt -Destination function:_OLD_VIRTUAL_PROMPT
    New-Variable -Name _PYTHON_VENV_PROMPT_PREFIX -Description "Python virtual environment prompt prefix" -Scope Global -Option ReadOnly -Visibility Public -Value $Prompt

    function global:prompt {
        Write-Host -NoNewline -ForegroundColor Green "($_PYTHON_VENV_PROMPT_PREFIX) "
        _OLD_VIRTUAL_PROMPT
    }
}

# Clear PYTHONHOME
if (Test-Path -Path Env:PYTHONHOME) {
    Copy-Item -Path Env:PYTHONHOME -Destination Env:_OLD_VIRTUAL_PYTHONHOME
    Remove-Item -Path Env:PYTHONHOME
}

# Add the venv to the PATH
Copy-Item -Path Env:PATH -Destination Env:_OLD_VIRTUAL_PATH
$Env:PATH = "$VenvExecDir$([System.IO.Path]::PathSeparator)$Env:PATH"

# SIG # Begin signature block
# MII27AYJKoZIhvcNAQcCoII23TCCNtkCAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCBALKwKRFIhr2RY
# IW/WJLd9pc8a9sj/IoThKU92fTfKsKCCG1wwggXMMIIDtKADAgECAhBUmNLR1FsZ
# lUgTecgRwIeZMA0GCSqGSIb3DQEBDAUAMHcxCzAJBgNVBAYTAlVTMR4wHAYDVQQK
# ExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xSDBGBgNVBAMTP01pY3Jvc29mdCBJZGVu
# dGl0eSBWZXJpZmljYXRpb24gUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgMjAy
# MDAeFw0yMDA0MTYxODM2MTZaFw00NTA0MTYxODQ0NDBaMHcxCzAJBgNVBAYTAlVT
# MR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xSDBGBgNVBAMTP01pY3Jv
# c29mdCBJZGVudGl0eSBWZXJpZmljYXRpb24gUm9vdCBDZXJ0aWZpY2F0ZSBBdXRo
# b3JpdHkgMjAyMDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBALORKgeD
# Bmf9np3gx8C3pOZCBH8Ppttf+9Va10Wg+3cL8IDzpm1aTXlT2KCGhFdFIMeiVPvH
# or+Kx24186IVxC9O40qFlkkN/76Z2BT2vCcH7kKbK/ULkgbk/WkTZaiRcvKYhOuD
# PQ7k13ESSCHLDe32R0m3m/nJxxe2hE//uKya13NnSYXjhr03QNAlhtTetcJtYmrV
# qXi8LW9J+eVsFBT9FMfTZRY33stuvF4pjf1imxUs1gXmuYkyM6Nix9fWUmcIxC70
# ViueC4fM7Ke0pqrrBc0ZV6U6CwQnHJFnni1iLS8evtrAIMsEGcoz+4m+mOJyoHI1
# vnnhnINv5G0Xb5DzPQCGdTiO0OBJmrvb0/gwytVXiGhNctO/bX9x2P29Da6SZEi3
# W295JrXNm5UhhNHvDzI9e1eM80UHTHzgXhgONXaLbZ7LNnSrBfjgc10yVpRnlyUK
# xjU9lJfnwUSLgP3B+PR0GeUw9gb7IVc+BhyLaxWGJ0l7gpPKWeh1R+g/OPTHU3mg
# trTiXFHvvV84wRPmeAyVWi7FQFkozA8kwOy6CXcjmTimthzax7ogttc32H83rwjj
# O3HbbnMbfZlysOSGM1l0tRYAe1BtxoYT2v3EOYI9JACaYNq6lMAFUSw0rFCZE4e7
# swWAsk0wAly4JoNdtGNz764jlU9gKL431VulAgMBAAGjVDBSMA4GA1UdDwEB/wQE
# AwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBTIftJqhSobyhmYBAcnz1AQ
# T2ioojAQBgkrBgEEAYI3FQEEAwIBADANBgkqhkiG9w0BAQwFAAOCAgEAr2rd5hnn
# LZRDGU7L6VCVZKUDkQKL4jaAOxWiUsIWGbZqWl10QzD0m/9gdAmxIR6QFm3FJI9c
# Zohj9E/MffISTEAQiwGf2qnIrvKVG8+dBetJPnSgaFvlVixlHIJ+U9pW2UYXeZJF
# xBA2CFIpF8svpvJ+1Gkkih6PsHMNzBxKq7Kq7aeRYwFkIqgyuH4yKLNncy2RtNwx
# AQv3Rwqm8ddK7VZgxCwIo3tAsLx0J1KH1r6I3TeKiW5niB31yV2g/rarOoDXGpc8
# FzYiQR6sTdWD5jw4vU8w6VSp07YEwzJ2YbuwGMUrGLPAgNW3lbBeUU0i/OxYqujY
# lLSlLu2S3ucYfCFX3VVj979tzR/SpncocMfiWzpbCNJbTsgAlrPhgzavhgplXHT2
# 6ux6anSg8Evu75SjrFDyh+3XOjCDyft9V77l4/hByuVkrrOj7FjshZrM77nq81YY
# uVxzmq/FdxeDWds3GhhyVKVB0rYjdaNDmuV3fJZ5t0GNv+zcgKCf0Xd1WF81E+Al
# GmcLfc4l+gcK5GEh2NQc5QfGNpn0ltDGFf5Ozdeui53bFv0ExpK91IjmqaOqu/dk
# ODtfzAzQNb50GQOmxapMomE2gj4d8yu8l13bS3g7LfU772Aj6PXsCyM2la+YZr9T
# 03u4aUoqlmZpxJTG9F9urJh4iIAGXKKy7aIwgga6MIIEoqADAgECAhMzAAjw7w9F
# p0Gh4pVjAAAACPDvMA0GCSqGSIb3DQEBDAUAMFoxCzAJBgNVBAYTAlVTMR4wHAYD
# VQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKzApBgNVBAMTIk1pY3Jvc29mdCBJ
# RCBWZXJpZmllZCBDUyBBT0MgQ0EgMDQwHhcNMjYxMDE5MDczODM5WhcNMjYxMDIy
# MDczODM5WjB8MQswCQYDVQQGEwJVUzEPMA0GA1UECBMGT3JlZ29uMRIwEAYDVQQH
# EwlCZWF2ZXJ0b24xIzAhBgNVBAoTGlB5dGhvbiBTb2Z0d2FyZSBGb3VuZGF0aW9u
# MSMwIQYDVQQDExpQeXRob24gU29mdHdhcmUgRm91bmRhdGlvbjCCAaIwDQYJKoZI
# hvcNAQEBBQADggGPADCCAYoCggGBAIeM6uA9EmUY6Oc7fGSGtbkmt8O8yC1YR0ww
# NZZmzu+hRa90v5cA+YcAHtnstVsOUPLPxpHgt+cUKEtCfz9I323LLbu8NWIyYUf7
# aB1SAQm4lvfKyDgytrhZTXmLVPqo9yi4/6Xbqz6ewbDdd9o4gIXA/lgVp4jEQZFl
# gWGFASnog5lmPk6w2yEeKStbEgw3pEX7UIa4HWpG46FB79fRyAIT2y7UT4WS9/JW
# WMswiXUgfFzHcHL+z0orPidEdv1mwRmmw6Bgtt5qnXajBW5260Ljt2bwdBXKavLx
# oguoM/AV7pTzGS3fBDFS23tznfx7VOYttGUzZtjy8L4ry1TALcDXe2bjhOJ5hXzG
# ijObfjdE96uRx/j3ECh1XF/bi+GDAOs0IiY2b4WpBHKg23eWyu0lKURn2VD9M8UR
# j5WGKhN0D/9Z5rqyGvfq02jM7EhxfiAaci0CbMlP3snvJZshtUT6LYgaz94qeuut
# jeCDLwUOI1iMJoYyFgeWdBhIM1cgoQIDAQABo4IB1TCCAdEwDAYDVR0TAQH/BAIw
# ADAOBgNVHQ8BAf8EBAMCB4AwPAYDVR0lBDUwMwYKKwYBBAGCN2EBAAYIKwYBBQUH
# AwMGGysGAQQBgjdhgqKNuwqmkohkgZH0oEWCk/3hbzAdBgNVHQ4EFgQUynN1IfEf
# pp97OttMQeftIRq3FWcwHwYDVR0jBBgwFoAUayVB3vtrfP0YgAotf492XapzPbgw
# ZwYDVR0fBGAwXjBcoFqgWIZWaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9w
# cy9jcmwvTWljcm9zb2Z0JTIwSUQlMjBWZXJpZmllZCUyMENTJTIwQU9DJTIwQ0El
# MjAwNC5jcmwwdAYIKwYBBQUHAQEEaDBmMGQGCCsGAQUFBzAChlhodHRwOi8vd3d3
# Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NlcnRzL01pY3Jvc29mdCUyMElEJTIwVmVy
# aWZpZWQlMjBDUyUyMEFPQyUyMENBJTIwMDQuY3J0MFQGA1UdIARNMEswSQYEVR0g
# ADBBMD8GCCsGAQUFBwIBFjNodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3Bz
# L0RvY3MvUmVwb3NpdG9yeS5odG0wDQYJKoZIhvcNAQEMBQADggIBAGXPieMG9RPj
# NybQNZRDu78vjpDLenNhy8U2ebH2Z1z85TY1vCx0a26X+XHZOxBG+CqWzX/N59Py
# WfRxbB/lznC72wcrMW8bYJqgZLSebp4FXkWxgwXXp/2WDWabzfAGKjmxKnbBC8M5
# K8qI/d9BS7eVrgbDF+ltmzW2AOnbE88zsyDMZJ4pD4sdyTGFnC39sriQ4+YoUjVE
# IEutTv0FPAFwR7YvN9B9YDYdRJjbjj8wAmDzDZFbmpBhOt1z51WHPAE0RgSj0Uu9
# CALeSeu806VmrWFyEGMMP/m6u4yy+iaqY3uCKbErkNLZVxA/mcFEzdhE9j3Vhu7J
# DL0239R3G+S7+qHGfMUf/sHKrHKo/BLGQGwMGkPHJwDFaO9KcqGZxCH6rJkWx0oO
# bgxhfAKcMgZBK8p2M0LpaR482CjWXn6MDriWNy8aC0wSq/l8PloGSY7zqlaOzeI1
# 5nqCIgUAJW+7qUc5ohBMvxgxQZfFXti0zrpTYMbgc4K1C14BBZG3uxf3wZwDG6yw
# Swr8DwVdB5kFKvceo9E0DNS5N61v4AmXe2KBar7zNK03Koo+RZ13x/QYSpzXmTc8
# 6Qe4PBWTy04r1S1izy3kmyj4MGjiUeWAZT0misd+W4cd0fLHCbsKaRNXGsLTbfKL
# 9yuYAtnBLTqjkvD4VlX9tQ88hp6+p7z/MIIHKDCCBRCgAwIBAgITMwAAABYxko2S
# AmV7mgAAAAAAFjANBgkqhkiG9w0BAQwFADBjMQswCQYDVQQGEwJVUzEeMBwGA1UE
# ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMTQwMgYDVQQDEytNaWNyb3NvZnQgSUQg
# VmVyaWZpZWQgQ29kZSBTaWduaW5nIFBDQSAyMDIxMB4XDTI2MDMyNjE4MTEyOVoX
# DTMxMDMyNjE4MTEyOVowWjELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jvc29m
# dCBDb3Jwb3JhdGlvbjErMCkGA1UEAxMiTWljcm9zb2Z0IElEIFZlcmlmaWVkIENT
# IEFPQyBDQSAwNDCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAMpV+sjb
# 6Akwz/RtDk5Uo1284BLMttRQy9e5/5WXtga6h89pkdWjeAwcXmKdP4YxKkhx8hn2
# q1dTVQbryvLy81tC04vg2bfSJ9emojX4HKIBYs7VhPRafMbtc866hN55aw1m/kWa
# PEKxF4Fm/LPLMLJdlu7URB8nFZMfh5tTC0CJb2uox/14OP/BiGIR8214lXdkV6Js
# PbO0Iev0mEV133tducIeBChMipzTZfnGVEq1QYFr7460cEGOIn+7AGfNOSq7gWOl
# mNB4m2uZ1r66vUJPaN+VYgH/Kmfu7tX229b3Alsli38fYS0nQY41bElntMS7yNY+
# Kd047eXM8/tS3NL+ZUNX3ge5xZqW2aytrrNIbwGgQnzsgzxBJvu9+b87jUWFiRS/
# z1YiPkRLY7iTHKIxJ973kIyK8K5itE/aEq/Ht6A8ytaAMMGTEwuCspk72FE1Qyby
# +TfDlv1KiAc7IlWHHIWbxoVd8jGCoMXhLSDhuFuGfOWOZKUIuW6YxlxcUYOOkXa0
# 2gC7dUYUZi0e1NI1Uq9mmAHdvjKdqgORs9/5aDjnbeO3hWd5qwGBmELWytkjY0Kc
# IEF+CMurTimoQoBYSiHJbYrb0pKSQe+3lVoTVpzdi36jKI7MxLnu1fBAMksa8uD8
# 5cU7RU29zMOd18h9dgTEH9pvY+4xLB3lUgE7AgMBAAGjggHcMIIB2DAOBgNVHQ8B
# Af8EBAMCAYYwEAYJKwYBBAGCNxUBBAMCAQAwHQYDVR0OBBYEFGslQd77a3z9GIAK
# LX+Pdl2qcz24MFQGA1UdIARNMEswSQYEVR0gADBBMD8GCCsGAQUFBwIBFjNodHRw
# Oi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL0RvY3MvUmVwb3NpdG9yeS5odG0w
# GQYJKwYBBAGCNxQCBAweCgBTAHUAYgBDAEEwEgYDVR0TAQH/BAgwBgEB/wIBADAf
# BgNVHSMEGDAWgBTZQSmwDw9jbO9p1/XNKZ6kSGow5jBwBgNVHR8EaTBnMGWgY6Bh
# hl9odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NybC9NaWNyb3NvZnQl
# MjBJRCUyMFZlcmlmaWVkJTIwQ29kZSUyMFNpZ25pbmclMjBQQ0ElMjAyMDIxLmNy
# bDB9BggrBgEFBQcBAQRxMG8wbQYIKwYBBQUHMAKGYWh0dHA6Ly93d3cubWljcm9z
# b2Z0LmNvbS9wa2lvcHMvY2VydHMvTWljcm9zb2Z0JTIwSUQlMjBWZXJpZmllZCUy
# MENvZGUlMjBTaWduaW5nJTIwUENBJTIwMjAyMS5jcnQwDQYJKoZIhvcNAQEMBQAD
# ggIBAAbVUF5UdNVEGWOVxkPciIzHA/IyNDIs1oSdW7mY4ObaFEEl8fwawEsBOasc
# BzwXjuOaTkKelQ+IiC4JvDpn8pWPgOyiKrbbw1Iswt7AV8YyuLu7A0YshILlxyny
# 9R0AMMQixLZ0T5Aj07CQOm/de5QPt36ECBwtVww6XUCx8Rm2xl6eEa9JhSub8W4u
# rQGoQYv0Zczlz4ej2ryj5Wf9L4ZZA3bL6CRE7XmzSQjTmdSmr904PiuL2uBWzq2K
# kR3RhmoaAP4Jk2JplasNM5Bs0+dx3YX2o6xRrbSaJiu6hPk/AkBoj5BCJMTZkl4w
# k6Q6nOFNSCpUxnBmJ0RRkoKq51p5ADTxbCeRAx8rIfNpTyPjxQtxTiFTC8yy/t9K
# 6s570YB1FAI+8XxZxIrmgMd7xVUkzi2/oooKb3UeovH0lqYGBEjpHZJ9jee7boQb
# Oe7SsKD/vr3PzFabg9VwLZlovpWUpWoWnU2w3xiozJ/m35tsZVvT2egUpwkb9TDI
# aXFGZAGV94FRDHn/K2XNnOwSeGskX19MB+N7yPc51fmUSd49MVAtGW/NkUGpRech
# 5Aq/d8dCML2bZ+j9nTUMgj+qnd3wuEZVRbQEIbTh9HAck0fyKqoIb+qh8y/6TKbY
# DEEOcfM2BGMvrQk4WIGCK6u8uFhA2EH3kpaiMUDqyFCeCCQxMIIHnjCCBYagAwIB
# AgITMwAAAAeHozSje6WOHAAAAAAABzANBgkqhkiG9w0BAQwFADB3MQswCQYDVQQG
# EwJVUzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMUgwRgYDVQQDEz9N
# aWNyb3NvZnQgSWRlbnRpdHkgVmVyaWZpY2F0aW9uIFJvb3QgQ2VydGlmaWNhdGUg
# QXV0aG9yaXR5IDIwMjAwHhcNMjEwNDAxMjAwNTIwWhcNMzYwNDAxMjAxNTIwWjBj
# MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMTQw
# MgYDVQQDEytNaWNyb3NvZnQgSUQgVmVyaWZpZWQgQ29kZSBTaWduaW5nIFBDQSAy
# MDIxMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAsvDArxmIKOLdVHpM
# SWxpCFUJtFL/ekr4weslKPdnF3cpTeuV8veqtmKVgok2rO0D05BpyvUDCg1wdsoE
# tuxACEGcgHfjPF/nZsOkg7c0mV8hpMT/GvB4uhDvWXMIeQPsDgCzUGzTvoi76YDp
# xDOxhgf8JuXWJzBDoLrmtThX01CE1TCCvH2sZD/+Hz3RDwl2MsvDSdX5rJDYVuR3
# bjaj2QfzZFmwfccTKqMAHlrz4B7ac8g9zyxlTpkTuJGtFnLBGasoOnn5NyYlf0xF
# 9/bjVRo4Gzg2Yc7KR7yhTVNiuTGH5h4eB9ajm1OCShIyhrKqgOkc4smz6obxO+Hx
# KeJ9bYmPf6KLXVNLz8UaeARo0BatvJ82sLr2gqlFBdj1sYfqOf00Qm/3B4XGFPDK
# /H04kteZEZsBRc3VT2d/iVd7OTLpSH9yCORV3oIZQB/Qr4nD4YT/lWkhVtw2v2s0
# TnRJubL/hFMIQa86rcaGMhNsJrhysLNNMeBhiMezU1s5zpusf54qlYu2v5sZ5zL0
# KvBDLHtL8F9gn6jOy3v7Jm0bbBHjrW5yQW7S36ALAt03QDpwW1JG1Hxu/FUXJbBO
# 2AwwVG4Fre+ZQ5Od8ouwt59FpBxVOBGfN4vN2m3fZx1gqn52GvaiBz6ozorgIEjn
# +PhUXILhAV5Q/ZgCJ0u2+ldFGjcCAwEAAaOCAjUwggIxMA4GA1UdDwEB/wQEAwIB
# hjAQBgkrBgEEAYI3FQEEAwIBADAdBgNVHQ4EFgQU2UEpsA8PY2zvadf1zSmepEhq
# MOYwVAYDVR0gBE0wSzBJBgRVHSAAMEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93d3cu
# bWljcm9zb2Z0LmNvbS9wa2lvcHMvRG9jcy9SZXBvc2l0b3J5Lmh0bTAZBgkrBgEE
# AYI3FAIEDB4KAFMAdQBiAEMAQTAPBgNVHRMBAf8EBTADAQH/MB8GA1UdIwQYMBaA
# FMh+0mqFKhvKGZgEByfPUBBPaKiiMIGEBgNVHR8EfTB7MHmgd6B1hnNodHRwOi8v
# d3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NybC9NaWNyb3NvZnQlMjBJZGVudGl0
# eSUyMFZlcmlmaWNhdGlvbiUyMFJvb3QlMjBDZXJ0aWZpY2F0ZSUyMEF1dGhvcml0
# eSUyMDIwMjAuY3JsMIHDBggrBgEFBQcBAQSBtjCBszCBgQYIKwYBBQUHMAKGdWh0
# dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2lvcHMvY2VydHMvTWljcm9zb2Z0JTIw
# SWRlbnRpdHklMjBWZXJpZmljYXRpb24lMjBSb290JTIwQ2VydGlmaWNhdGUlMjBB
# dXRob3JpdHklMjAyMDIwLmNydDAtBggrBgEFBQcwAYYhaHR0cDovL29uZW9jc3Au
# bWljcm9zb2Z0LmNvbS9vY3NwMA0GCSqGSIb3DQEBDAUAA4ICAQB/JSqe/tSr6t1m
# CttXI0y6XmyQ41uGWzl9xw+WYhvOL47BV09Dgfnm/tU4ieeZ7NAR5bguorTCNr58
# HOcA1tcsHQqt0wJsdClsu8bpQD9e/al+lUgTUJEV80Xhco7xdgRrehbyhUf4pkeA
# hBEjABvIUpD2LKPho5Z4DPCT5/0TlK02nlPwUbv9URREhVYCtsDM+31OFU3fDV8B
# mQXv5hT2RurVsJHZgP4y26dJDVF+3pcbtvh7R6NEDuYHYihfmE2HdQRq5jRvLE1E
# b59PYwISFCX2DaLZ+zpU4bX0I16ntKq4poGOFaaKtjIA1vRElItaOKcwtc04CBrX
# SfyL2Op6mvNIxTk4OaswIkTXbFL81ZKGD+24uMCwo/pLNhn7VHLfnxlMVzHQVL+b
# Ha9KhTyzwdG/L6uderJQn0cGpLQMStUuNDArxW2wF16QGZ1NtBWgKA8Kqv48M8Hf
# FqNifN6+zt6J0GwzvU8g0rYGgTZR8zDEIJfeZxwWDHpSxB5FJ1VVU1LIAtB7o9PX
# bjXzGifaIMYTzU4YKt4vMNwwBmetQDHhdAtTPplOXrnI9SI6HeTtjDD3iUN/7ygb
# ahmYOHk7VB7fwT4ze+ErCbMh6gHV1UuXPiLciloNxH6K4aMfZN1oLVk6YFeIJEok
# uPgNPa6EnTiOL60cPqfny+Fq8UiuZzGCGuYwghriAgEBMHEwWjELMAkGA1UEBhMC
# VVMxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjErMCkGA1UEAxMiTWlj
# cm9zb2Z0IElEIFZlcmlmaWVkIENTIEFPQyBDQSAwNAITMwAI8O8PRadBoeKVYwAA
# AAjw7zANBglghkgBZQMEAgEFAKCBsjAZBgkqhkiG9w0BCQMxDAYKKwYBBAGCNwIB
# BDAcBgorBgEEAYI3AgELMQ4wDAYKKwYBBAGCNwIBFTAvBgkqhkiG9w0BCQQxIgQg
# Kld7dFLdvZygPQIm94QeWCWq09RhnYWpKs5R8/oLpjgwRgYKKwYBBAGCNwIBDDE4
# MDagMIAuAFAAeQB0AGgAbwBuACAAMwAuADEANAAuADkAIAAoAGUAYQBkADgAMgBj
# ADAAKaECgAAwDQYJKoZIhvcNAQEBBQAEggGAGG7X9+gJfXAj6gSlLpqUnJ1kCjzv
# 3uCpVmbpv8Ogyz02Kzv8EkFrFnQuF0sCvz6sEh6BZ6qWLFs2fYwDvGm5jya+WkrV
# nSHgO9MSzmnWdfC6hNRtyvhrHIgvlN9T4hBvnCKeXkB4s2GAkc3JlRqH0PQoQSRN
# NYf+dBTX/PEC+KlMZxdKQF0LzJ75G31XKwi0X0IoR22bsrFe5iz4PSVOD8cMyh0D
# sCz73LcFCvAeVT6WQFTnbPgiphU9cGn7HdHfMaTKgAx4iQIM7bDjDvv3tRJUTbCI
# Wh0W8QgpMbYo/CNvNB/xOUoWX9N9GHOgS4vZOaw300Va+EUqJL12wVRVrh/JCxMr
# PRRJWy9UTsJqI4XYAfi8D2YJyZzvGuNFB0fuRApltgZ0+wpSHuNVwv2xeNk6V9aW
# p861ig5wojbdVE8VvCcnRZhQVUdE0nAzzfbDBYM3fIbVcgLQTicDuyOKNqij/Naf
# bdvEY7/DP5PRPDEVVME/KmKBb+xa2yw8rjS+oYIYETCCGA0GCisGAQQBgjcDAwEx
# ghf9MIIX+QYJKoZIhvcNAQcCoIIX6jCCF+YCAQMxDzANBglghkgBZQMEAgEFADCC
# AWIGCyqGSIb3DQEJEAEEoIIBUQSCAU0wggFJAgEBBgorBgEEAYRZCgMBMDEwDQYJ
# YIZIAWUDBAIBBQAEIKuGdjJ+MDdWpIQxsi98psLmjyfnJp8rHxnRDy6UGdHiAgZq
# 16hZPogYEzIwMjYxMDIxMDczOTM4Ljg2M1owBIACAfSggeGkgd4wgdsxCzAJBgNV
# BAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4w
# HAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xJTAjBgNVBAsTHE1pY3Jvc29m
# dCBBbWVyaWNhIE9wZXJhdGlvbnMxJzAlBgNVBAsTHm5TaGllbGQgVFNTIEVTTjo3
# QTAwLTA1RTAtRDk0NzE1MDMGA1UEAxMsTWljcm9zb2Z0IFB1YmxpYyBSU0EgVGlt
# ZSBTdGFtcGluZyBBdXRob3JpdHmggg8hMIIHgjCCBWqgAwIBAgITMwAAAAXlzw//
# Zi7JhwAAAAAABTANBgkqhkiG9w0BAQwFADB3MQswCQYDVQQGEwJVUzEeMBwGA1UE
# ChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMUgwRgYDVQQDEz9NaWNyb3NvZnQgSWRl
# bnRpdHkgVmVyaWZpY2F0aW9uIFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9yaXR5IDIw
# MjAwHhcNMjAxMTE5MjAzMjMxWhcNMzUxMTE5MjA0MjMxWjBhMQswCQYDVQQGEwJV
# UzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNy
# b3NvZnQgUHVibGljIFJTQSBUaW1lc3RhbXBpbmcgQ0EgMjAyMDCCAiIwDQYJKoZI
# hvcNAQEBBQADggIPADCCAgoCggIBAJ5851Jj/eDFnwV9Y7UGIqMcHtfnlzPREwW9
# ZUZHd5HBXXBvf7KrQ5cMSqFSHGqg2/qJhYqOQxwuEQXG8kB41wsDJP5d0zmLYKAY
# 8Zxv3lYkuLDsfMuIEqvGYOPURAH+Ybl4SJEESnt0MbPEoKdNihwM5xGv0rGofJ1q
# OYSTNcc55EbBT7uq3wx3mXhtVmtcCEr5ZKTkKKE1CxZvNPWdGWJUPC6e4uRfWHIh
# ZcgCsJ+sozf5EeH5KrlFnxpjKKTavwfFP6XaGZGWUG8TZaiTogRoAlqcevbiqioU
# z1Yt4FRK53P6ovnUfANjIgM9JDdJ4e0qiDRm5sOTiEQtBLGd9Vhd1MadxoGcHrRC
# sS5rO9yhv2fjJHrmlQ0EIXmp4DhDBieKUGR+eZ4CNE3ctW4uvSDQVeSp9h1SaPV8
# UWEfyTxgGjOsRpeexIveR1MPTVf7gt8hY64XNPO6iyUGsEgt8c2PxF87E+CO7A28
# TpjNq5eLiiunhKbq0XbjkNoU5JhtYUrlmAbpxRjb9tSreDdtACpm3rkpxp7AQndn
# I0Shu/fk1/rE3oWsDqMX3jjv40e8KN5YsJBnczyWB4JyeeFMW3JBfdeAKhzohFe8
# U5w9WuvcP1E8cIxLoKSDzCCBOu0hWdjzKNu8Y5SwB1lt5dQhABYyzR3dxEO/T1K/
# BVF3rV69AgMBAAGjggIbMIICFzAOBgNVHQ8BAf8EBAMCAYYwEAYJKwYBBAGCNxUB
# BAMCAQAwHQYDVR0OBBYEFGtpKDo1L0hjQM972K9J6T7ZPdshMFQGA1UdIARNMEsw
# SQYEVR0gADBBMD8GCCsGAQUFBwIBFjNodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20v
# cGtpb3BzL0RvY3MvUmVwb3NpdG9yeS5odG0wEwYDVR0lBAwwCgYIKwYBBQUHAwgw
# GQYJKwYBBAGCNxQCBAweCgBTAHUAYgBDAEEwDwYDVR0TAQH/BAUwAwEB/zAfBgNV
# HSMEGDAWgBTIftJqhSobyhmYBAcnz1AQT2ioojCBhAYDVR0fBH0wezB5oHegdYZz
# aHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9jcmwvTWljcm9zb2Z0JTIw
# SWRlbnRpdHklMjBWZXJpZmljYXRpb24lMjBSb290JTIwQ2VydGlmaWNhdGUlMjBB
# dXRob3JpdHklMjAyMDIwLmNybDCBlAYIKwYBBQUHAQEEgYcwgYQwgYEGCCsGAQUF
# BzAChnVodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NlcnRzL01pY3Jv
# c29mdCUyMElkZW50aXR5JTIwVmVyaWZpY2F0aW9uJTIwUm9vdCUyMENlcnRpZmlj
# YXRlJTIwQXV0aG9yaXR5JTIwMjAyMC5jcnQwDQYJKoZIhvcNAQEMBQADggIBAF+I
# dsd+bbVaFXXnTHho+k7h2ESZJRWluLE0Oa/pO+4ge/XEizXvhs0Y7+KVYyb4nHlu
# gBesnFqBGEdC2IWmtKMyS1OWIviwpnK3aL5JedwzbeBF7POyg6IGG/XhhJ3UqWeW
# TO+Czb1c2NP5zyEh89F72u9UIw+IfvM9lzDmc2O2END7MPnrcjWdQnrLn1Ntday7
# JSyrDvBdmgbNnCKNZPmhzoa8PccOiQljjTW6GePe5sGFuRHzdFt8y+bN2neF7Zu8
# hTO1I64XNGqst8S+w+RUdie8fXC1jKu3m9KGIqF4aldrYBamyh3g4nJPj/LR2CBa
# LyD+2BuGZCVmoNR/dSpRCxlot0i79dKOChmoONqbMI8m04uLaEHAv4qwKHQ1vBzb
# V/nG89LDKbRSSvijmwJwxRxLLpMQ/u4xXxFfR4f/gksSkbJp7oqLwliDm/h+w0aJ
# /U5ccnYhYb7vPKNMN+SZDWycU5ODIRfyoGl59BsXR/HpRGtiJquOYGmvA/pk5vC1
# lcnbeMrcWD/26ozePQ/TWfNXKBOmkFpvPE8CH+EeGGWzqTCjdAsno2jzTeNSxlx3
# glDGJgcdz5D/AAxw9Sdgq/+rY7jjgs7X6fqPTXPmaCAJKVHAP19oEjJIBwD1LyHb
# aEgBxFCogYSOiUIr0Xqcr1nJfiWG2GwYe6ZoAF1bMIIHlzCCBX+gAwIBAgITMwAA
# AF3GOXhPQH0z/QAAAAAAXTANBgkqhkiG9w0BAQwFADBhMQswCQYDVQQGEwJVUzEe
# MBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3Nv
# ZnQgUHVibGljIFJTQSBUaW1lc3RhbXBpbmcgQ0EgMjAyMDAeFw0yNjA5MTcxODU3
# MjFaFw0yNzA5MTQxODU3MjFaMIHbMQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2Fz
# aGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENv
# cnBvcmF0aW9uMSUwIwYDVQQLExxNaWNyb3NvZnQgQW1lcmljYSBPcGVyYXRpb25z
# MScwJQYDVQQLEx5uU2hpZWxkIFRTUyBFU046N0EwMC0wNUUwLUQ5NDcxNTAzBgNV
# BAMTLE1pY3Jvc29mdCBQdWJsaWMgUlNBIFRpbWUgU3RhbXBpbmcgQXV0aG9yaXR5
# MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAnfeo3PEUD4ms6oOxxnr5
# 0+xXsYIC89uEGj2MMjXhwsmk31IRN7u7diRWxUr1PfVqZUQSwpJ0NlB4Np5ImpQ2
# wio7MbAEu7GT9fw2VlGJBdanG87gNKKRr91ipU6R+HS2vsmGSAkX1uKg/LAuUttZ
# 9HMk4IZPt2jyP/UsUxNB8MEsDb+1jVODSksDGZytN2ImF85UbZ84sAGzkIwapHjV
# jLIOcI07nfunp/8psYD+l4G6TTV+QI9KyBKpHGg945xF5Bju8jo7auPPnHvMSj2h
# zGfnZKw8FlPqmAepuXClZRvpvtqHPLze6bgMLRIy9R/SGyb/T7723i9CavF5VxM1
# 1REBvBcNQtX1QvZ3zWqMPpk9u4sKgnacVVrB6V2+z1gl5PiaEbQ6k/7sii90m7bu
# yp1nhPxtVaup3up25vs0kAOaOJPOnzG4uZCsB0NH8+EENAWCDP6PzxLnVmbKJwdy
# dCRW0Q8qL+SyIdJXMrKx85c4MTNr0hmnjmatRWP8lMij6edIYWPLxIWx5dJOYn4a
# VEmF5GeFAoZRKxzPQw1jfUQnEA7xJ0+olpHbPfQbxi8k2BOLSlQzHR5vAa2q4X+X
# 7KPgXV/PRIOYJnUuKSqmk0Sy2LEkfBrfZ9v35qpv5QYFH8DWGvShGglZsBuJMIXV
# LIyT1siEJ89fYmAsMM39jM0CAwEAAaOCAcswggHHMB0GA1UdDgQWBBRD2wpdmU7w
# fh5W7f3lqGjIf1rnuTAfBgNVHSMEGDAWgBRraSg6NS9IY0DPe9ivSek+2T3bITBs
# BgNVHR8EZTBjMGGgX6BdhltodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3Bz
# L2NybC9NaWNyb3NvZnQlMjBQdWJsaWMlMjBSU0ElMjBUaW1lc3RhbXBpbmclMjBD
# QSUyMDIwMjAuY3JsMHkGCCsGAQUFBwEBBG0wazBpBggrBgEFBQcwAoZdaHR0cDov
# L3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9jZXJ0cy9NaWNyb3NvZnQlMjBQdWJs
# aWMlMjBSU0ElMjBUaW1lc3RhbXBpbmclMjBDQSUyMDIwMjAuY3J0MAwGA1UdEwEB
# /wQCMAAwFgYDVR0lAQH/BAwwCgYIKwYBBQUHAwgwDgYDVR0PAQH/BAQDAgeAMGYG
# A1UdIARfMF0wUQYMKwYBBAGCN0yDfQEBMEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93
# d3cubWljcm9zb2Z0LmNvbS9wa2lvcHMvRG9jcy9SZXBvc2l0b3J5Lmh0bTAIBgZn
# gQwBBAIwDQYJKoZIhvcNAQEMBQADggIBAELIRPApwy7XbiTh4WhtQ6gKfJeKC1/p
# HJUNX+CMvvdgV8Feg7o4paA4m/pCs0+vNxB/LSQLrSzjZgJrhpRaATTi2ih2jaUc
# n12Kaxfa+BjVHMysfIYjGY9GhIlaZR1m+sPaktieppf8rIz/lX8+Wx7ozgaFEYnU
# 076R6tCbF3VxO5/BybUrXHMIMocNtQZ7eQtHcwE2wgotoGzmz0uU2gJiL7nWvl4f
# atCUctOWBCYOH90rTvdUON0+VtABpas/mhXurQypBVWbITPirdcHiEqAe2KcUHMC
# eazBsUvhHfIsQ0KxHeIPADpYJkVRNMxBCighDoitsHLQX74/4cIk8xKPo6CQ5eY1
# AUIuWY6DiXQ4FmjVOg+gIEMieDyOQiukmMTdMc6YK3kYH/XbyxkSUKzs6Gr8HkkB
# ZiU3syLke3CadevKdo/hFXSPZuLWWvV5hjo3ad8x4Pg3oWouH9vP+wmrrDzVKa/p
# nHRYRkCic+lJKCMc+4IFc8w9g5SCkoU+n4nCxgwCO68qPyacs1OGxlVq34wfwwdC
# ILMR7ctM5voCfP6r7G67X4PUfXW5DJwo6ZHed1zroRj4jIaxZ5L9INnuS9cValdw
# r+NYIm5nk5sQAJzBp7u6UbvyGbtWXYPNtx2Jvv8RNYTTzPqaFaJ2YzmaYOts5AbD
# vyRH2vNhyZNKMYIHQzCCBz8CAQEweDBhMQswCQYDVQQGEwJVUzEeMBwGA1UEChMV
# TWljcm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUHVibGlj
# IFJTQSBUaW1lc3RhbXBpbmcgQ0EgMjAyMAITMwAAAF3GOXhPQH0z/QAAAAAAXTAN
# BglghkgBZQMEAgEFAKCCBJwwEQYLKoZIhvcNAQkQAg8xAgUAMBoGCSqGSIb3DQEJ
# AzENBgsqhkiG9w0BCRABBDAcBgkqhkiG9w0BCQUxDxcNMjYxMDIxMDczOTM4WjAv
# BgkqhkiG9w0BCQQxIgQgoksQpngmiqre/Oegp43+KYRQKcirgQHZcFZAsQ/9wZ8w
# gbkGCyqGSIb3DQEJEAIvMYGpMIGmMIGjMIGgBCAtNWRLb+nz7kJioWWnRqK5hoUA
# Q15v3MvrF73cxAJKXTB8MGWkYzBhMQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWlj
# cm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUHVibGljIFJT
# QSBUaW1lc3RhbXBpbmcgQ0EgMjAyMAITMwAAAF3GOXhPQH0z/QAAAAAAXTCCA14G
# CyqGSIb3DQEJEAISMYIDTTCCA0mhggNFMIIDQTCCAikCAQEwggEJoYHhpIHeMIHb
# MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
# bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSUwIwYDVQQLExxN
# aWNyb3NvZnQgQW1lcmljYSBPcGVyYXRpb25zMScwJQYDVQQLEx5uU2hpZWxkIFRT
# UyBFU046N0EwMC0wNUUwLUQ5NDcxNTAzBgNVBAMTLE1pY3Jvc29mdCBQdWJsaWMg
# UlNBIFRpbWUgU3RhbXBpbmcgQXV0aG9yaXR5oiMKAQEwBwYFKw4DAhoDFQC9p7q1
# tbGAzwdowGwPEcSazJaQ9aBnMGWkYzBhMQswCQYDVQQGEwJVUzEeMBwGA1UEChMV
# TWljcm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUHVibGlj
# IFJTQSBUaW1lc3RhbXBpbmcgQ0EgMjAyMDANBgkqhkiG9w0BAQsFAAIFAO6Cz50w
# IhgPMjAyNjEwMjEwNTQzNTdaGA8yMDI2MTAyMjA1NDM1N1owdDA6BgorBgEEAYRZ
# CgQBMSwwKjAKAgUA7oLPnQIBADAHAgEAAgINWjAHAgEAAgISZDAKAgUA7oQhHQIB
# ADA2BgorBgEEAYRZCgQCMSgwJjAMBgorBgEEAYRZCgMCoAowCAIBAAIDB6EgoQow
# CAIBAAIDAYagMA0GCSqGSIb3DQEBCwUAA4IBAQCC4FJWpuvKf9mXdjMuCEXjhncB
# wL3Gd+EKhapihjA33jrrdAKM7t/vgxQoG7a7WxFpFW499/4UYVQUEwp925Xx7CNG
# EwAYC89gmsIxbGHeql9HZn5JYb0rmT6CQ1UvfFrBgtV0IdBigvemRf0Z7a1v8ufh
# q/TZ3oQW5rO+P0PhbWW3NvkZbHeRjJbocG0ii32CdoULNFtkDTUazQlNthx/pHli
# X9xdwLc90jll54w8rk43D6iXpPiv4ccWrbkuy06tDbj8qUQJxJMEHsBalcRdSlmd
# CN6KoGUd93q4aFUn0/ANdrB3V9+seU9UyPZbg0BSyl0D1g+bkZ1U5l9Ng3M4MA0G
# CSqGSIb3DQEBAQUABIICAF5rB9faBWCp6heoAFcR5NmLvZdYwnaJtL7GnHbxNDL7
# 4evPMe4oynNQbZFqbI9IcW4bmv+T8saj0yflhNkecg7lBpkgtHma/qQ2Loa3be+e
# VVh5T6ZpCuU5+MHrYhL8QPscoebHwvpZx7iGPrwKy3TDJ0k0k6herZ+kLlqPyUdj
# SHYGUlNGshVZiJ1MXAoF1beVpXoDgVopIsHcs6BMqzzb8JmTmVAHXQpKVVLWzlR8
# KqcaD6MfdG2VtmiWWSC+MTTbhJAa1II0obnssqXuyxBwJASSc5qzbm0/SYB/BG2V
# pk3EQQQJyJ+ugAwC/CtGWtr9E0HJnUMU9p4BiD7psmdYQ/eyMOyWsq+1X1eHsrmE
# yjZF8kbZ2udUfdCEKjy362Ax5ASjiERpeSBYetvk8Ki2BafBSOuikHHZOa4bxO8Z
# fk6WtcaKFA5sw+Fi8PIiDjSP4Qiee6AUxKhj3WpvW6tmbopFAoh2F2kgbtuhT+0d
# JcEOlLyaPzJg6u37OBzn2UFuYNVJIEqi3jRA46glWBm+iiXmxbhVIooM5hWXP9y1
# QOL6HV/x5aSKeQ8/WwKZyBocIBJmmfqy/C8rhRpPtbvdJK5eBU71jH8JfkX0k5DH
# zjxCl1Eq+2L3QA6Peb2aYYkgz/5i5XfvJZTDiWq50QkI2razfvZ7odFp+9yx884n
# SIG # End signature block
