@echo off

:: Set PYTHONBUILDDIR to the root of your python directory
:: Or, if you just want to build the installer for PythonScript with an existing python312.dll,
:: set PYTHONBUILDDIR to a path containing a directory, which contains the python312.dll


SET ORIGINALDIR=%CD%

CD /d %~dp0

IF NOT EXIST "buildPaths.bat" (
	echo You need to copy/rename buildPaths.bat.orig to buildPaths.bat, and edit it to your local configuration
	goto error
	)

SET PYTHONSCRIPTDLLDIR=bin
SET INST_TEMP_DIR=temp
SET NAME_ADDON=
SET MSI_ARCH=x86

CALL buildPaths.bat

IF "%1"=="x64" SET PYTHONBUILDDIR=%PYTHONBUILDDIR_X64%
IF "%1"=="x64" SET PYTHONSCRIPTDLLDIR=x64
IF "%1"=="x64" SET INST_TEMP_DIR=temp64
IF "%1"=="x64" SET NAME_ADDON=_x64
IF "%1"=="x64" SET MSI_ARCH=x64

IF NOT EXIST "%PYTHONBUILDDIR%\python.exe" (
	echo Your PYTHONBUILDDIR in buildPaths.bat does not contain python.exe.  Please set PYTHONBUILDDIR to the root of a built Python 3.12
	goto error
	)

IF NOT EXIST "%PYTHONBUILDDIR%\python312.dll" (
	echo Your PYTHONBUILDDIR in buildPaths.bat does not contain python312.dll.  Please set PYTHONBUILDDIR to the root of a built Python 3.12
	goto error
	)

mkdir %INST_TEMP_DIR%

%PYTHONBUILDDIR%\python extractVersion.py > %INST_TEMP_DIR%\version.txt
SET /p PYTHONSCRIPTVERSION= < %INST_TEMP_DIR%\version.txt


cd ..\packages
echo add UI extension globally
wix extension add -g WixToolset.UI.wixext
wix extension list -g
cd ..\installer


echo Compiling PythonScript installer
wix build pythonscript.wxs -o build\%PYTHONSCRIPTVERSION%\PythonScript_%PYTHONSCRIPTVERSION%%NAME_ADDON%.msi -d version=%PYTHONSCRIPTVERSION% -d baseDir=.. -d pythonDir=%PYTHONBUILDDIR% -d variantDir=%PYTHONSCRIPTDLLDIR% -d Platform=%MSI_ARCH% -d nameAddon=%NAME_ADDON% -arch %MSI_ARCH% -ext "WixToolset.UI.wixext"
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo.
echo.
echo Installer created - build\%PYTHONSCRIPTVERSION%\PythonScript_%PYTHONSCRIPTVERSION%%NAME_ADDON%.msi

goto end

:error
echo Error!

:end

CD /d %ORIGINALDIR%

