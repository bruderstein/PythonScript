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
    $env:VIRTUAL_ENV_PROMPT = $Prompt
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
# MII0BQYJKoZIhvcNAQcCoIIz9jCCM/ICAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCBnL745ElCYk8vk
# dBtMuQhLeWJ3ZGfzKW4DHCYzAn+QB6CCG9IwggXMMIIDtKADAgECAhBUmNLR1FsZ
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
# 03u4aUoqlmZpxJTG9F9urJh4iIAGXKKy7aIwggb+MIIE5qADAgECAhMzAAKlFFgo
# 8MZcu1p7AAAAAqUUMA0GCSqGSIb3DQEBDAUAMFoxCzAJBgNVBAYTAlVTMR4wHAYD
# VQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKzApBgNVBAMTIk1pY3Jvc29mdCBJ
# RCBWZXJpZmllZCBDUyBBT0MgQ0EgMDIwHhcNMjUwMjA0MDQzOTE4WhcNMjUwMjA3
# MDQzOTE4WjB8MQswCQYDVQQGEwJVUzEPMA0GA1UECBMGT3JlZ29uMRIwEAYDVQQH
# EwlCZWF2ZXJ0b24xIzAhBgNVBAoTGlB5dGhvbiBTb2Z0d2FyZSBGb3VuZGF0aW9u
# MSMwIQYDVQQDExpQeXRob24gU29mdHdhcmUgRm91bmRhdGlvbjCCAaIwDQYJKoZI
# hvcNAQEBBQADggGPADCCAYoCggGBAM0+8Om1I2jOLEAcjJV18w+wi9VDTfS8oEaX
# kaAFVx5RpBFMUQl8rALAnyXgfRjSO4ACoyvo7v9tCNYtEzc/7eT01z/pVf/WHGw1
# iccrUMIx6nS+yWR998L9lZfBQPpyAziTWcRrnt+C1hRDb/7GqLjL6xujXhI6VWwu
# P7OGA9DcbeCGiA9LgH7C0iMxLsVcro7WG+Zbv3bFnW6bBCvjJhzsy134ZKL5WJpV
# IF8U6OHGFr612DuFZN5qX9qU/S4qibpxf4VFvm0YLPIQfvNPNYVbLGZ1fnOrglPD
# 7yoEFnNvjIF4eGlKhdaVGEOajCY6qTEyntqrDw3FkoebbSPZsO6jgTeRgikQFM4A
# eKtQ21+0ISd4yQS1qZBa6k1yGjgfv0q0MF2tLO/wDyC4JMZLBITFDOfLo1aFMFXL
# ZipHTsq21g/VlSHCQ3n+uI3F3XqtT6en5qeAMeLcJ0NflERYTn1auNghCYyDlzeV
# s9n42/3t87X17w3cHi/GUZlu+dmSiQIDAQABo4ICGTCCAhUwDAYDVR0TAQH/BAIw
# ADAOBgNVHQ8BAf8EBAMCB4AwPAYDVR0lBDUwMwYKKwYBBAGCN2EBAAYIKwYBBQUH
# AwMGGysGAQQBgjdhgqKNuwqmkohkgZH0oEWCk/3hbzAdBgNVHQ4EFgQUHg/yH95s
# crpDPlCqvm1yj6v7twwwHwYDVR0jBBgwFoAUJEWZoXeQKnzDyoOwbmQWhCr4LGcw
# ZwYDVR0fBGAwXjBcoFqgWIZWaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9w
# cy9jcmwvTWljcm9zb2Z0JTIwSUQlMjBWZXJpZmllZCUyMENTJTIwQU9DJTIwQ0El
# MjAwMi5jcmwwgaUGCCsGAQUFBwEBBIGYMIGVMGQGCCsGAQUFBzAChlhodHRwOi8v
# d3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NlcnRzL01pY3Jvc29mdCUyMElEJTIw
# VmVyaWZpZWQlMjBDUyUyMEFPQyUyMENBJTIwMDIuY3J0MC0GCCsGAQUFBzABhiFo
# dHRwOi8vb25lb2NzcC5taWNyb3NvZnQuY29tL29jc3AwZgYDVR0gBF8wXTBRBgwr
# BgEEAYI3TIN9AQEwQTA/BggrBgEFBQcCARYzaHR0cDovL3d3dy5taWNyb3NvZnQu
# Y29tL3BraW9wcy9Eb2NzL1JlcG9zaXRvcnkuaHRtMAgGBmeBDAEEATANBgkqhkiG
# 9w0BAQwFAAOCAgEAbKNGvVBi7AmP6/ArF3MEiLOizuTKcVxI9LfwZtPn0cFUaTdi
# pOrR8aROpTJtx0QMywVXEadCYbsopJSASdVCbTH+3Q65Q4ZU69phlIDwskPGl0fo
# nWjz4iU+BMaGevmlQRmPBfBUshKlb5cVz6BqXpql7dK+M8LPNHw0szLUqzMqFt/Z
# rdTdOLoabWE70/TQqEbLddoB0+osgL+4evhv6S000yanBf5tZVctd8AGQEzo/hKj
# LOBb55PwWxh5Nmz8Q8uAhU1Z+s1i7AP53zzUeaY4iwAvvXDD6f/J3MCJrA2pGwCf
# XxNCuf0cMB/+5Q998/guVhQzRIyukmgtVjWvETEA6bXo69W1r6PnG3I9bqj1kkCB
# 3Qifl/OGO4GlsLxTxxQKe6UmDDcER8YwB9+VtEIWBBT1MdBGLgc0NekxSgICsTzG
# BOa8QMozV8oelupdJ8k64OzvrWAZF5aGOquA74T1Tj+xoJz53I4nhwCPwYv60hND
# bPRUhmDyyDcGzEAAUDrDbWq8DjYKg+H/JJtNcpW86+iHBmBHAmdA3I/AhklyaKYK
# q7bInqFNYbWGLWHCSAOWhSC4sq4FCq/Bh1vaMVOKonJOeATFD+ID2FH/7287sT8y
# Pa71vDq1y1ctW/MbZ5s4N2ZarC1VVTBQ2ySKYfDlUYcx/U1crt/noma4Urcwggda
# MIIFQqADAgECAhMzAAAABJZQS9Lb7suIAAAAAAAEMA0GCSqGSIb3DQEBDAUAMGMx
# CzAJBgNVBAYTAlVTMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xNDAy
# BgNVBAMTK01pY3Jvc29mdCBJRCBWZXJpZmllZCBDb2RlIFNpZ25pbmcgUENBIDIw
# MjEwHhcNMjEwNDEzMTczMTUyWhcNMjYwNDEzMTczMTUyWjBaMQswCQYDVQQGEwJV
# UzEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSswKQYDVQQDEyJNaWNy
# b3NvZnQgSUQgVmVyaWZpZWQgQ1MgQU9DIENBIDAyMIICIjANBgkqhkiG9w0BAQEF
# AAOCAg8AMIICCgKCAgEA4c6g6DOiY6bAOwCPbBlQF2tjo3ckUZuab5ZorMnRp4rO
# mwZDiTbIpzFkZ/k8k4ivBJV1w5/b/oykI+eXAqaaxMdyAO0ModnEW7InfQ+rTkyk
# EzHxRbCNg6KDsTnYc/YdL7IIiJli8k51upaHLL7CYm9YNc0SFYvlaFj2O0HjO9y/
# NRmcWNjamZOlRjxW2cWgUsUdazSHgRCek87V2bM/17b+o8WXUW91IpggRasmiZ65
# WEFHXKbyhm2LbhBK6ZWmQoFeE+GWrKWCGK/q/4RiTaMNhHXWvWv+//I58UtOxVi3
# DaK1fQ6YLyIIGHzD4CmtcrGivxupq/crrHunGNB7//Qmul2ZP9HcOmY/aptgUnwT
# +20g/A37iDfuuVw6yS2Lo0/kp/jb+J8vE4FMqIiwxGByL482PMVBC3qd/NbFQa8M
# mj6ensU+HEqv9ar+AbcKwumbZqJJKmQrGaSNdWfk2NodgcWOmq7jyhbxwZOjnLj0
# /bwnsUNcNAe09v+qiozyQQes8A3UXPcRQb8G+c0yaO2ICifWTK7ySuyUJ88k1mtN
# 22CNftbjitiAeafoZ9Vmhn5Rfb+S/K5arVvTcLukt5PdTDQxl557EIE6A+6XFBpd
# sjOzkLzdEh7ELk8PVPMjQfPCgKtJ84c17fd2C9+pxF1lEQUFXY/YtCL+Nms9cWUC
# AwEAAaOCAg4wggIKMA4GA1UdDwEB/wQEAwIBhjAQBgkrBgEEAYI3FQEEAwIBADAd
# BgNVHQ4EFgQUJEWZoXeQKnzDyoOwbmQWhCr4LGcwVAYDVR0gBE0wSzBJBgRVHSAA
# MEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2lvcHMv
# RG9jcy9SZXBvc2l0b3J5Lmh0bTAZBgkrBgEEAYI3FAIEDB4KAFMAdQBiAEMAQTAS
# BgNVHRMBAf8ECDAGAQH/AgEAMB8GA1UdIwQYMBaAFNlBKbAPD2Ns72nX9c0pnqRI
# ajDmMHAGA1UdHwRpMGcwZaBjoGGGX2h0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9w
# a2lvcHMvY3JsL01pY3Jvc29mdCUyMElEJTIwVmVyaWZpZWQlMjBDb2RlJTIwU2ln
# bmluZyUyMFBDQSUyMDIwMjEuY3JsMIGuBggrBgEFBQcBAQSBoTCBnjBtBggrBgEF
# BQcwAoZhaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9jZXJ0cy9NaWNy
# b3NvZnQlMjBJRCUyMFZlcmlmaWVkJTIwQ29kZSUyMFNpZ25pbmclMjBQQ0ElMjAy
# MDIxLmNydDAtBggrBgEFBQcwAYYhaHR0cDovL29uZW9jc3AubWljcm9zb2Z0LmNv
# bS9vY3NwMA0GCSqGSIb3DQEBDAUAA4ICAQBnLThdlbMNIokdKtzSa8io+pEO95Cc
# 3VOyY/hQsIIcdMyk2hJOzLt/M1WXfQyElDk/QtyLzX63TdOb5J+nO8t0pzzwi7ZY
# vMiNqKvAQO50sMOJn3T3hCPppxNNhoGFVxz2UyiQ4b2vOrcsLK9TOEFXWbUMJObR
# 9PM0wZsABIhu4k6VVLxEDe0GSeQX/ZE7PHfTg44Luft4IKqYmnv1Cuosp3glFYsV
# egLnMWZUZ8UtO9F8QCiAouJYhL5OlCksgDb9ve/HQhLFnelfg6dQubIFsqB9IlCo
# nYKJZ/HaMZvYtA7y9EORK4cxlvTetCXAHayiSXH0ueE/T92wVG0csv5VdUyj6yVr
# m22vlKYAkXINKvDOB8+s4h+TgShlUa2ACu2FWn7JzlTSbpk0IE8REuYmkuyE/BTk
# k93WDMx7PwLnn4J+5fkvbjjQ08OewfpMhh8SuPdQKqmZ40I4W2UyJKMMTbet16JF
# imSqDChgnCB6lwlpe0gfbo97U7prpbfBKp6B2k2f7Y+TjWrQYN+OdcPOyQAdxGGP
# BwJSaJG3ohdklCxgAJ5anCxeYl7SjQ5Eua6atjIeVhN0KfPLFPpYz5CQU+JC2H79
# x4d/O6YOFR9aYe54/CGup7dRUIfLSv1/j0DPc6Elf3YyWxloWj8yeY3kHrZFaAlR
# MwhAXyPQ3rEX9zCCB54wggWGoAMCAQICEzMAAAAHh6M0o3uljhwAAAAAAAcwDQYJ
# KoZIhvcNAQEMBQAwdzELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jvc29mdCBD
# b3Jwb3JhdGlvbjFIMEYGA1UEAxM/TWljcm9zb2Z0IElkZW50aXR5IFZlcmlmaWNh
# dGlvbiBSb290IENlcnRpZmljYXRlIEF1dGhvcml0eSAyMDIwMB4XDTIxMDQwMTIw
# MDUyMFoXDTM2MDQwMTIwMTUyMFowYzELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1p
# Y3Jvc29mdCBDb3Jwb3JhdGlvbjE0MDIGA1UEAxMrTWljcm9zb2Z0IElEIFZlcmlm
# aWVkIENvZGUgU2lnbmluZyBQQ0EgMjAyMTCCAiIwDQYJKoZIhvcNAQEBBQADggIP
# ADCCAgoCggIBALLwwK8ZiCji3VR6TElsaQhVCbRS/3pK+MHrJSj3Zxd3KU3rlfL3
# qrZilYKJNqztA9OQacr1AwoNcHbKBLbsQAhBnIB34zxf52bDpIO3NJlfIaTE/xrw
# eLoQ71lzCHkD7A4As1Bs076Iu+mA6cQzsYYH/Cbl1icwQ6C65rU4V9NQhNUwgrx9
# rGQ//h890Q8JdjLLw0nV+ayQ2Fbkd242o9kH82RZsH3HEyqjAB5a8+Ae2nPIPc8s
# ZU6ZE7iRrRZywRmrKDp5+TcmJX9MRff241UaOBs4NmHOyke8oU1TYrkxh+YeHgfW
# o5tTgkoSMoayqoDpHOLJs+qG8Tvh8SnifW2Jj3+ii11TS8/FGngEaNAWrbyfNrC6
# 9oKpRQXY9bGH6jn9NEJv9weFxhTwyvx9OJLXmRGbAUXN1U9nf4lXezky6Uh/cgjk
# Vd6CGUAf0K+Jw+GE/5VpIVbcNr9rNE50Sbmy/4RTCEGvOq3GhjITbCa4crCzTTHg
# YYjHs1NbOc6brH+eKpWLtr+bGecy9CrwQyx7S/BfYJ+ozst7+yZtG2wR461uckFu
# 0t+gCwLdN0A6cFtSRtR8bvxVFyWwTtgMMFRuBa3vmUOTnfKLsLefRaQcVTgRnzeL
# zdpt32cdYKp+dhr2ogc+qM6K4CBI5/j4VFyC4QFeUP2YAidLtvpXRRo3AgMBAAGj
# ggI1MIICMTAOBgNVHQ8BAf8EBAMCAYYwEAYJKwYBBAGCNxUBBAMCAQAwHQYDVR0O
# BBYEFNlBKbAPD2Ns72nX9c0pnqRIajDmMFQGA1UdIARNMEswSQYEVR0gADBBMD8G
# CCsGAQUFBwIBFjNodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL0RvY3Mv
# UmVwb3NpdG9yeS5odG0wGQYJKwYBBAGCNxQCBAweCgBTAHUAYgBDAEEwDwYDVR0T
# AQH/BAUwAwEB/zAfBgNVHSMEGDAWgBTIftJqhSobyhmYBAcnz1AQT2ioojCBhAYD
# VR0fBH0wezB5oHegdYZzaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9j
# cmwvTWljcm9zb2Z0JTIwSWRlbnRpdHklMjBWZXJpZmljYXRpb24lMjBSb290JTIw
# Q2VydGlmaWNhdGUlMjBBdXRob3JpdHklMjAyMDIwLmNybDCBwwYIKwYBBQUHAQEE
# gbYwgbMwgYEGCCsGAQUFBzAChnVodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtp
# b3BzL2NlcnRzL01pY3Jvc29mdCUyMElkZW50aXR5JTIwVmVyaWZpY2F0aW9uJTIw
# Um9vdCUyMENlcnRpZmljYXRlJTIwQXV0aG9yaXR5JTIwMjAyMC5jcnQwLQYIKwYB
# BQUHMAGGIWh0dHA6Ly9vbmVvY3NwLm1pY3Jvc29mdC5jb20vb2NzcDANBgkqhkiG
# 9w0BAQwFAAOCAgEAfyUqnv7Uq+rdZgrbVyNMul5skONbhls5fccPlmIbzi+OwVdP
# Q4H55v7VOInnmezQEeW4LqK0wja+fBznANbXLB0KrdMCbHQpbLvG6UA/Xv2pfpVI
# E1CRFfNF4XKO8XYEa3oW8oVH+KZHgIQRIwAbyFKQ9iyj4aOWeAzwk+f9E5StNp5T
# 8FG7/VEURIVWArbAzPt9ThVN3w1fAZkF7+YU9kbq1bCR2YD+MtunSQ1Rft6XG7b4
# e0ejRA7mB2IoX5hNh3UEauY0byxNRG+fT2MCEhQl9g2i2fs6VOG19CNep7SquKaB
# jhWmirYyANb0RJSLWjinMLXNOAga10n8i9jqeprzSMU5ODmrMCJE12xS/NWShg/t
# uLjAsKP6SzYZ+1Ry358ZTFcx0FS/mx2vSoU8s8HRvy+rnXqyUJ9HBqS0DErVLjQw
# K8VtsBdekBmdTbQVoCgPCqr+PDPB3xajYnzevs7eidBsM71PINK2BoE2UfMwxCCX
# 3mccFgx6UsQeRSdVVVNSyALQe6PT12418xon2iDGE81OGCreLzDcMAZnrUAx4XQL
# Uz6ZTl65yPUiOh3k7Yww94lDf+8oG2oZmDh5O1Qe38E+M3vhKwmzIeoB1dVLlz4i
# 3IpaDcR+iuGjH2TdaC1ZOmBXiCRKJLj4DT2uhJ04ji+tHD6n58vhavFIrmcxgheJ
# MIIXhQIBATBxMFoxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
# cG9yYXRpb24xKzApBgNVBAMTIk1pY3Jvc29mdCBJRCBWZXJpZmllZCBDUyBBT0Mg
# Q0EgMDICEzMAAqUUWCjwxly7WnsAAAACpRQwDQYJYIZIAWUDBAIBBQCggcgwGQYJ
# KoZIhvcNAQkDMQwGCisGAQQBgjcCAQQwHAYKKwYBBAGCNwIBCzEOMAwGCisGAQQB
# gjcCARUwLwYJKoZIhvcNAQkEMSIEIGcBno/ti9PCrR9sXrajsTvlHQvGxbk63JiI
# URJByQuGMFwGCisGAQQBgjcCAQwxTjBMoEaARABCAHUAaQBsAHQAOgAgAFIAZQBs
# AGUAYQBzAGUAXwB2ADMALgAxADIALgA5AF8AMgAwADIANQAwADIAMAA0AC4AMAAx
# oQKAADANBgkqhkiG9w0BAQEFAASCAYBPqeGNLCO1x2d5/5YeFIn62QD/k4SL+HAq
# TJWnO64E9WJhbKuCLBMCQunmG3iUS31CFxZmDA1VIIugNhSdRpDA92cKPTdO+K8g
# d7mWr+BBHfKZSsckqkgETwDEWDl+rEcRBAZz8ukqAdIwiWQqOjK6Yvq8/RXPa/Pa
# j4O9T9b5Ttv4z/UT4WqilQVrYLWClMbQy1ghScUDuEwrep6idivNNHaXETiq+Mar
# aWKh0smQ28R48AplB/DzYtspcPgUTqohPlj9raA5POiVy8FsbdnuPvyD4ES+K370
# bJmDuC3eTtJ/EllJUSulaU+INQnIhS5DXoSXUYtBjhr0NyrYG/WNEEhNYYxLHthy
# 1ZZQ3eFW6mGlfX4RqkSEg0/NAWpRYRVPh7MwnTrx5QcW/hR13Z3oxqOypoTVZlfq
# 0MLyyH+JxHw0+XceahlqNBWQAlrQ8cSP2mfS9kjix32ocdCsd8kiKt89ukqIxHtd
# ZAMhjn0ATwOa0lS0YibfJai8r+7lhFChghSeMIIUmgYKKwYBBAGCNwMDATGCFIow
# ghSGBgkqhkiG9w0BBwKgghR3MIIUcwIBAzEPMA0GCWCGSAFlAwQCAQUAMIIBXwYL
# KoZIhvcNAQkQAQSgggFOBIIBSjCCAUYCAQEGCisGAQQBhFkKAwEwMTANBglghkgB
# ZQMEAgEFAAQgHBb5my5znYRAZx1pFgy7P4+c347qmmaEtxfaTBr84wECBmebw28S
# RxgRMjAyNTAyMDQxNTUwMDAuOFowBIACAfSggeCkgd0wgdoxCzAJBgNVBAYTAlVT
# MRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQK
# ExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xJTAjBgNVBAsTHE1pY3Jvc29mdCBBbWVy
# aWNhIE9wZXJhdGlvbnMxJjAkBgNVBAsTHVRoYWxlcyBUU1MgRVNOOkU0NjItOTZG
# MC00NDJFMTUwMwYDVQQDEyxNaWNyb3NvZnQgUHVibGljIFJTQSBUaW1lIFN0YW1w
# aW5nIEF1dGhvcml0eaCCDyAwggeCMIIFaqADAgECAhMzAAAABeXPD/9mLsmHAAAA
# AAAFMA0GCSqGSIb3DQEBDAUAMHcxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xSDBGBgNVBAMTP01pY3Jvc29mdCBJZGVudGl0eSBW
# ZXJpZmljYXRpb24gUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgMjAyMDAeFw0y
# MDExMTkyMDMyMzFaFw0zNTExMTkyMDQyMzFaMGExCzAJBgNVBAYTAlVTMR4wHAYD
# VQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xMjAwBgNVBAMTKU1pY3Jvc29mdCBQ
# dWJsaWMgUlNBIFRpbWVzdGFtcGluZyBDQSAyMDIwMIICIjANBgkqhkiG9w0BAQEF
# AAOCAg8AMIICCgKCAgEAnnznUmP94MWfBX1jtQYioxwe1+eXM9ETBb1lRkd3kcFd
# cG9/sqtDlwxKoVIcaqDb+omFio5DHC4RBcbyQHjXCwMk/l3TOYtgoBjxnG/eViS4
# sOx8y4gSq8Zg49REAf5huXhIkQRKe3Qxs8Sgp02KHAznEa/Ssah8nWo5hJM1xznk
# RsFPu6rfDHeZeG1Wa1wISvlkpOQooTULFm809Z0ZYlQ8Lp7i5F9YciFlyAKwn6yj
# N/kR4fkquUWfGmMopNq/B8U/pdoZkZZQbxNlqJOiBGgCWpx69uKqKhTPVi3gVErn
# c/qi+dR8A2MiAz0kN0nh7SqINGbmw5OIRC0EsZ31WF3Uxp3GgZwetEKxLms73KG/
# Z+MkeuaVDQQheangOEMGJ4pQZH55ngI0Tdy1bi69INBV5Kn2HVJo9XxRYR/JPGAa
# M6xGl57Ei95HUw9NV/uC3yFjrhc087qLJQawSC3xzY/EXzsT4I7sDbxOmM2rl4uK
# K6eEpurRduOQ2hTkmG1hSuWYBunFGNv21Kt4N20AKmbeuSnGnsBCd2cjRKG79+TX
# +sTehawOoxfeOO/jR7wo3liwkGdzPJYHgnJ54UxbckF914AqHOiEV7xTnD1a69w/
# UTxwjEugpIPMIIE67SFZ2PMo27xjlLAHWW3l1CEAFjLNHd3EQ79PUr8FUXetXr0C
# AwEAAaOCAhswggIXMA4GA1UdDwEB/wQEAwIBhjAQBgkrBgEEAYI3FQEEAwIBADAd
# BgNVHQ4EFgQUa2koOjUvSGNAz3vYr0npPtk92yEwVAYDVR0gBE0wSzBJBgRVHSAA
# MEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2lvcHMv
# RG9jcy9SZXBvc2l0b3J5Lmh0bTATBgNVHSUEDDAKBggrBgEFBQcDCDAZBgkrBgEE
# AYI3FAIEDB4KAFMAdQBiAEMAQTAPBgNVHRMBAf8EBTADAQH/MB8GA1UdIwQYMBaA
# FMh+0mqFKhvKGZgEByfPUBBPaKiiMIGEBgNVHR8EfTB7MHmgd6B1hnNodHRwOi8v
# d3d3Lm1pY3Jvc29mdC5jb20vcGtpb3BzL2NybC9NaWNyb3NvZnQlMjBJZGVudGl0
# eSUyMFZlcmlmaWNhdGlvbiUyMFJvb3QlMjBDZXJ0aWZpY2F0ZSUyMEF1dGhvcml0
# eSUyMDIwMjAuY3JsMIGUBggrBgEFBQcBAQSBhzCBhDCBgQYIKwYBBQUHMAKGdWh0
# dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2lvcHMvY2VydHMvTWljcm9zb2Z0JTIw
# SWRlbnRpdHklMjBWZXJpZmljYXRpb24lMjBSb290JTIwQ2VydGlmaWNhdGUlMjBB
# dXRob3JpdHklMjAyMDIwLmNydDANBgkqhkiG9w0BAQwFAAOCAgEAX4h2x35ttVoV
# dedMeGj6TuHYRJklFaW4sTQ5r+k77iB79cSLNe+GzRjv4pVjJviceW6AF6ycWoEY
# R0LYhaa0ozJLU5Yi+LCmcrdovkl53DNt4EXs87KDogYb9eGEndSpZ5ZM74LNvVzY
# 0/nPISHz0Xva71QjD4h+8z2XMOZzY7YQ0Psw+etyNZ1CesufU211rLslLKsO8F2a
# Bs2cIo1k+aHOhrw9xw6JCWONNboZ497mwYW5EfN0W3zL5s3ad4Xtm7yFM7Ujrhc0
# aqy3xL7D5FR2J7x9cLWMq7eb0oYioXhqV2tgFqbKHeDick+P8tHYIFovIP7YG4Zk
# JWag1H91KlELGWi3SLv10o4KGag42pswjybTi4toQcC/irAodDW8HNtX+cbz0sMp
# tFJK+KObAnDFHEsukxD+7jFfEV9Hh/+CSxKRsmnuiovCWIOb+H7DRon9TlxydiFh
# vu88o0w35JkNbJxTk4MhF/KgaXn0GxdH8elEa2Imq45gaa8D+mTm8LWVydt4ytxY
# P/bqjN49D9NZ81coE6aQWm88TwIf4R4YZbOpMKN0CyejaPNN41LGXHeCUMYmBx3P
# kP8ADHD1J2Cr/6tjuOOCztfp+o9Nc+ZoIAkpUcA/X2gSMkgHAPUvIdtoSAHEUKiB
# hI6JQivRepyvWcl+JYbYbBh7pmgAXVswggeWMIIFfqADAgECAhMzAAAASv24Vcdi
# qsmJAAAAAABKMA0GCSqGSIb3DQEBDAUAMGExCzAJBgNVBAYTAlVTMR4wHAYDVQQK
# ExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xMjAwBgNVBAMTKU1pY3Jvc29mdCBQdWJs
# aWMgUlNBIFRpbWVzdGFtcGluZyBDQSAyMDIwMB4XDTI0MTEyNjE4NDg1NVoXDTI1
# MTExOTE4NDg1NVowgdoxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9u
# MRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRp
# b24xJTAjBgNVBAsTHE1pY3Jvc29mdCBBbWVyaWNhIE9wZXJhdGlvbnMxJjAkBgNV
# BAsTHVRoYWxlcyBUU1MgRVNOOkU0NjItOTZGMC00NDJFMTUwMwYDVQQDEyxNaWNy
# b3NvZnQgUHVibGljIFJTQSBUaW1lIFN0YW1waW5nIEF1dGhvcml0eTCCAiIwDQYJ
# KoZIhvcNAQEBBQADggIPADCCAgoCggIBAOg5EGGes2JcNQG8RZBTcHNFdAWlwgqN
# f/ndNWCos2pSvW1/HxqyFElX++aWCTq4WJqHyDIe8Ab/I/pCKZTimIsa7VavmkMA
# HsyNMARXRRXjsjQiMp00MM9+N8+SFlWdP+3Dl09SA5PXyLCOdZGSh2uEj3/axZh/
# V7R9fz9RMbngBrJARfwLyNTpdlb9lvRA5HvHLMXD4F05AzRvSOZYxIzPaWVM3/ZI
# MPD6PAlaQ9fzhW+OvI3GlqbIv8nam6GtyiZz6MlJZ6vnJoKen5KamJjzNxIQK9Wg
# KmzJ5IlKw+ENeQ079F6GTotc410YReccVyi69ZuZOcWZR6e5pz4pohuu0+/3uQ7h
# Rbe12wLDglvDdWauY3TXi6zC7Qb5EcipZE0rKepyUF0eEX9nvO0673kIiYy7FvmU
# OOil3XAyNVqGsSNCc8qYnDkAW3FPfPNt6f0wK2HVaSGLmJV6tPdt3H/foputgVRR
# vXl4ZvutNUbF3VpR+0FDGc2w9t92gD/C4gdKQ65Cv8m1KWLOg6JACfa9GpuDRV7u
# ywvg3yBb3kSGTggWhxreEidxtlYJBSKACxXE4Y/q6O6F/gU97K9C+ey84v0JlNlJ
# EwsFdaijOvmqm84VeAcbMUY30HXrasxb8SOCB39CAbj6GGDIieAF02tpqMpn7noy
# tjWYC++ZlsIrAgMBAAGjggHLMIIBxzAdBgNVHQ4EFgQUF3TWibYuFnuOhK1P0zZy
# DnlhlXowHwYDVR0jBBgwFoAUa2koOjUvSGNAz3vYr0npPtk92yEwbAYDVR0fBGUw
# YzBhoF+gXYZbaHR0cDovL3d3dy5taWNyb3NvZnQuY29tL3BraW9wcy9jcmwvTWlj
# cm9zb2Z0JTIwUHVibGljJTIwUlNBJTIwVGltZXN0YW1waW5nJTIwQ0ElMjAyMDIw
# LmNybDB5BggrBgEFBQcBAQRtMGswaQYIKwYBBQUHMAKGXWh0dHA6Ly93d3cubWlj
# cm9zb2Z0LmNvbS9wa2lvcHMvY2VydHMvTWljcm9zb2Z0JTIwUHVibGljJTIwUlNB
# JTIwVGltZXN0YW1waW5nJTIwQ0ElMjAyMDIwLmNydDAMBgNVHRMBAf8EAjAAMBYG
# A1UdJQEB/wQMMAoGCCsGAQUFBwMIMA4GA1UdDwEB/wQEAwIHgDBmBgNVHSAEXzBd
# MFEGDCsGAQQBgjdMg30BATBBMD8GCCsGAQUFBwIBFjNodHRwOi8vd3d3Lm1pY3Jv
# c29mdC5jb20vcGtpb3BzL0RvY3MvUmVwb3NpdG9yeS5odG0wCAYGZ4EMAQQCMA0G
# CSqGSIb3DQEBDAUAA4ICAQAwKc0w11m8o/6/Ob+gM9fbCT7cCnRObpANBaz8SX3y
# A0NFiS1tRu81yFyCWN0uXQh2716+Ww65i+uzJZ1NDNESGqZucaUvHo7lnUaZR6pE
# YsIQeYZvu5xHcZvmHSgnKCD8WxqiNbfBNEtGbe7sNf/S80VGq3bkx0NDysmUJ7mJ
# ByxsfN//kUeNMNYHpoiBbQv1H7zWPVERwnwem1qfdKot041izDtNxcHKbr2deUbm
# zFR/3h91mryYyweKw1nezc3kgJIk6ghFABTaqxhW54zr4gTFGypAGv5hY/ZtQ5Om
# ceEm17U+cAqHV95N9JtHEgdpwpVC20eGMljufzKEpU8JzD7xVzDllJM3f0gEW8O1
# 1Reli7YGR1QSgMmeDBSsKygj/IV3aM3ZHjl90btdvsHsEZPAJ6P3/c65uBvwf8Du
# ru7Jgd3kfDnq6oqD7e97nxTFrEgsF4Uhs/MaY6ugyoht/bA0Txdgh57ASCZr+3Tz
# IGAisbJG+xoiJB26mu/byAKkLPZ2OLO14UfDSkQClSGI/f/F5HmOUtRtGEIQsEJU
# VLbx2TGNKWJjlwX1qY0BvmWiMH3dO7QdIaayQYecySyLrLH7CD97Rq+KOuLakWvP
# jiLT95i1gnbJSnAWORyMxwtm9DZ2rAGNW6s3036LbH5Ya1fqV0sjdwLN+nSjE6qR
# hjGCA9QwggPQAgEBMHgwYTELMAkGA1UEBhMCVVMxHjAcBgNVBAoTFU1pY3Jvc29m
# dCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFB1YmxpYyBSU0EgVGlt
# ZXN0YW1waW5nIENBIDIwMjACEzMAAABK/bhVx2KqyYkAAAAAAEowDQYJYIZIAWUD
# BAIBBQCgggEtMBoGCSqGSIb3DQEJAzENBgsqhkiG9w0BCRABBDAvBgkqhkiG9w0B
# CQQxIgQgKzed9E7hTqzOm9DvCGJTMRw8VbkImyZ4RPTcUcSplZkwgd0GCyqGSIb3
# DQEJEAIvMYHNMIHKMIHHMIGgBCBmeym1pc2NAEJHS3Vv4AMYspezxH/VqbL/sXXv
# LsPhCjB8MGWkYzBhMQswCQYDVQQGEwJVUzEeMBwGA1UEChMVTWljcm9zb2Z0IENv
# cnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUHVibGljIFJTQSBUaW1lc3Rh
# bXBpbmcgQ0EgMjAyMAITMwAAAEr9uFXHYqrJiQAAAAAASjAiBCAZhqR+LorzfAJx
# I6219Oe9oW8cVDoih7oqEFvRVXh8BjANBgkqhkiG9w0BAQsFAASCAgCdP6W/pipO
# iiPBIEiwauOezR9CLHH8fUwrELK2E4Ej+JxrWhNY2aoYzKDvgs78zTCuqcEL7JVR
# D16ohPKCJPy6+lIO769TboPoYO7qkAlG3S6czokR264Qw//ocQQhKbgJh8pAbr9m
# aQwRw+pQORNlsgHsUPYFDY5dgoKWja/52CqzzDnuOEvzSrXcXbWEAb3lRS1KIUN+
# 8y9VH/6WBg4gf/B07Te84Ktqg/6XafVoGzeZP1YW6AQ2eqyg/9ieVCkeFUkCj9NI
# Ud3h1FDzeeDRSV8mw7yu48lt8pjJbWY1iJAnl7p8BwrOQVJBOH17AZwFqU7XG+hJ
# TGqb0fUEgSX8dfybEvZo6ts1VGdJmQdZoYjVDvPaccOdORakejwCm9VHFql0nhUN
# llq53/ftAVHgSG403TjNfD43ZQhoDbPLZiUB58ujhC+vrWDrbnKgdL8Bh5Khru2T
# sJlAEw48RWBGn92YR1Q4oFuXOfBSLKJ2FEVhp3tL2ncyp8ynlyM2KjaSsaCVFGCS
# awia+fswKwvH0VyU+gtr1dB10SAVMZvqR2xmc3skKSxqFclvk2W2kApKB1OqZgOo
# JRFWTz/zxVl77yc228ZzFrL8BJwLCXiLHn7DtD6kiwYrHAW04Ix3n8WGBaI5ULaa
# noz5eyimOTWgxuLUCMQ1GGwFJdVbGJrQ+Q==
# SIG # End signature block
