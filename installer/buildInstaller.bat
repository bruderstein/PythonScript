@echo off

:: Set PYTHONBUILDDIR to the root of your python directory
:: Or, if you just want to build the installer for PythonScript with an existing python27.dll,
:: set PYTHONBUILDDIR to a path containing a directory, which contains the python27.dll


SET ORIGINALDIR=%CD%

CD /d %~dp0

IF NOT EXIST "buildPaths.bat" (
	echo You need to copy/rename buildPaths.bat.orig to buildPaths.bat, and edit it to your local configuration
	goto error
	)

SET PYTHONSCRIPTDLLDIR=bin
SET INST_TEMP_DIR=temp
SET NAME_ADDON=""

CALL buildPaths.bat

IF "%1"=="x64" SET PYTHONBUILDDIR=%PYTHONBUILDDIR_X64%
IF "%1"=="x64" SET PYTHONSCRIPTDLLDIR=x64
IF "%1"=="x64" SET INST_TEMP_DIR=temp64
IF "%1"=="x64" SET NAME_ADDON=_x64

IF NOT EXIST "%PYTHONBUILDDIR%\python.exe" (
	echo Your PYTHONBUILDDIR in buildPaths.bat does not contain python.exe.  Please set PYTHONBUILDDIR to the root of a built Python 2.7
	goto error
	)

IF NOT EXIST "%PYTHONBUILDDIR%\python27.dll" (
	echo Your PYTHONBUILDDIR in buildPaths.bat does not contain python27.dll.  Please set PYTHONBUILDDIR to the root of a built Python 2.7
	goto error
	)

mkdir %INST_TEMP_DIR%

%PYTHONBUILDDIR%\python extractVersion.py > %INST_TEMP_DIR%\version.txt
SET /p PYTHONSCRIPTVERSION= < %INST_TEMP_DIR%\version.txt


echo Generating WiX information for ..\pythonlib\full
heat dir ..\pythonlib\full -ag -cg CG_PythonLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\fullLib.wxs 
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling python lib WiX source
candle %INST_TEMP_DIR%\fullLib.wxs -o %INST_TEMP_DIR%\fullLib.wixobj -dpylibSource=..\pythonlib\full
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\full_dll%NAME_ADDON%
heat dir ..\pythonlib\full_dll%NAME_ADDON% -ag -cg CG_PythonLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wxs 
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling python pyd lib WiX source
candle %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wxs -o %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wixobj -dpylibSource=..\pythonlib\full_dll%NAME_ADDON%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\extra
heat dir ..\pythonlib\extra -ag -cg CG_PythonExtraLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\extra.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling extra lib WiX source
candle %INST_TEMP_DIR%\extra.wxs -o %INST_TEMP_DIR%\extra.wixobj -dpylibSource=..\pythonlib\extra
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\extra_dll%NAME_ADDON%
heat dir ..\pythonlib\extra_dll%NAME_ADDON% -ag -cg CG_PythonExtraLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\extra_dll%NAME_ADDON%.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling extra lib WiX source
candle %INST_TEMP_DIR%\extra_dll%NAME_ADDON%.wxs -o %INST_TEMP_DIR%\extra_dll%NAME_ADDON%.wixobj -dpylibSource=..\pythonlib\extra_dll%NAME_ADDON%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\tcl
heat dir ..\pythonlib\tcl -ag -cg CG_PythonTclTkLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\tcl.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling tcl lib WiX source
candle %INST_TEMP_DIR%\tcl.wxs -o %INST_TEMP_DIR%\tcl.wixobj -dpylibSource=..\pythonlib\tcl
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\PythonScript\scripts\Samples
heat dir ..\scripts\Samples -ag -cg CG_SampleScripts -dr D_PythonScript -var var.scriptSource -t changeDirSampleScripts.xsl -o %INST_TEMP_DIR%\sampleScripts.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling Sample Scripts WiX source
candle %INST_TEMP_DIR%\sampleScripts.wxs -o %INST_TEMP_DIR%\sampleScripts.wixobj -dscriptSource=..\scripts\Samples
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)


echo Generating WiX information for ..\PythonScript\python_tests
heat dir ..\PythonScript\python_tests -ag -cg CG_UnitTests -dr D_PythonScript -var var.unittestSource -t changeDirTests.xsl -o %INST_TEMP_DIR%\unittests.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling Unit test WiX source
candle %INST_TEMP_DIR%\unittests.wxs -o %INST_TEMP_DIR%\unittests.wixobj -dunittestSource=..\pythonscript\python_tests
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)



echo Compiling main PythonScript installer
candle pythonscript.wxs -o %INST_TEMP_DIR%\pythonscript.wixobj -dversion=%PYTHONSCRIPTVERSION% -dbaseDir=.. -dpythonDir=%PYTHONBUILDDIR% -dvariantDir=%PYTHONSCRIPTDLLDIR%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)


echo Linking installer - generating MSI
IF NOT EXIST "build\%PYTHONSCRIPTVERSION%" (
	mkdir build\%PYTHONSCRIPTVERSION%
)


light %INST_TEMP_DIR%\pythonscript.wixobj %INST_TEMP_DIR%\fullLib.wixobj %INST_TEMP_DIR%\extra.wixobj %INST_TEMP_DIR%\unittests.wixobj %INST_TEMP_DIR%\tcl.wixobj %INST_TEMP_DIR%\sampleScripts.wixobj -o build\%PYTHONSCRIPTVERSION%\PythonScript_%PYTHONSCRIPTVERSION%%NAME_ADDON%.msi -ext WixUIExtension
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

