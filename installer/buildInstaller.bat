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


echo Generating WiX information for ..\pythonlib\full
heat dir ..\pythonlib\full -ag -cg CG_PythonLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\fullLib.wxs 
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling python lib WiX source
wix build %INST_TEMP_DIR%\fullLib.wxs -o %INST_TEMP_DIR%\fullLib.wixlib -d pylibSource=..\pythonlib\full -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\full_dll%NAME_ADDON%
heat dir ..\pythonlib\full_dll%NAME_ADDON% -ag -cg CG_PythonLibDll -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wxs 
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling python pyd lib WiX source
wix build %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wxs -o %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wixlib -d pylibSource=..\pythonlib\full_dll%NAME_ADDON% -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\tcl
heat dir ..\pythonlib\tcl -ag -cg CG_PythonTclTkLib -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\tcl.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling tcl lib WiX source
wix build %INST_TEMP_DIR%\tcl.wxs -o %INST_TEMP_DIR%\tcl.wixlib -d pylibSource=..\pythonlib\tcl -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\pythonlib\tcl_dll%NAME_ADDON%
heat dir ..\pythonlib\tcl_dll%NAME_ADDON% -ag -cg CG_PythonTclTkLibDll -dr D_PythonScript -var var.pylibSource -t changeDirLib.xsl -o %INST_TEMP_DIR%\tcl_dll%NAME_ADDON%.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling tcl lib WiX source
wix build %INST_TEMP_DIR%\tcl_dll%NAME_ADDON%.wxs -o %INST_TEMP_DIR%\tcl_dll%NAME_ADDON%.wixlib -d pylibSource=..\pythonlib\tcl_dll%NAME_ADDON% -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\PythonScript\scripts\Samples
heat dir ..\scripts\Samples -ag -cg CG_SampleScripts -dr D_PythonScript -var var.scriptSource -t changeDirSampleScripts.xsl -o %INST_TEMP_DIR%\sampleScripts.wxs
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling Sample Scripts WiX source
wix build %INST_TEMP_DIR%\sampleScripts.wxs -o %INST_TEMP_DIR%\sampleScripts.wixlib -d scriptSource=..\scripts\Samples -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)


echo Generating WiX information for ..\PythonScript\python_tests
heat dir ..\PythonScript\python_tests -ag -cg CG_UnitTests -dr D_PythonScript -var var.unittestSource -t changeDirTests.xsl -o %INST_TEMP_DIR%\unittests.wxs -platform=%NAME_ADDON%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling Unit test WiX source
wix build %INST_TEMP_DIR%\unittests.wxs -o %INST_TEMP_DIR%\unittests.wixlib -d unittestSource=..\PythonScript\python_tests -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Generating WiX information for ..\docs\build\html
heat dir ..\docs\build\html -ag -cg CG_HtmlDocs -dr D_PythonScript -var var.htmldocsSource -t changeDirHtmlDoc.xsl -o %INST_TEMP_DIR%\htmldoc.wxs -platform=%NAME_ADDON%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

echo Compiling Html doc WiX source
wix build %INST_TEMP_DIR%\htmldoc.wxs -o %INST_TEMP_DIR%\htmldoc.wixlib -d htmldocsSource=..\docs\build\html -arch %MSI_ARCH%
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)

cd ..\packages
echo add UI extension globally
wix extension add -g WixToolset.UI.wixext
wix extension list -g
cd ..\installer


echo Compiling main PythonScript installer
wix build pythonscript.wxs -o %INST_TEMP_DIR%\pythonscript.wixlib -d version=%PYTHONSCRIPTVERSION% -d baseDir=.. -d pythonDir=%PYTHONBUILDDIR% -d variantDir=%PYTHONSCRIPTDLLDIR% -d Platform=%MSI_ARCH% -arch %MSI_ARCH% -ext "WixToolset.UI.wixext"
if NOT [%ERRORLEVEL%]==[0] (
 goto error
)


echo Linking installer - generating MSI
IF NOT EXIST "build\%PYTHONSCRIPTVERSION%" (
	mkdir build\%PYTHONSCRIPTVERSION%
)

wix build %INST_TEMP_DIR%\pythonscript.wixlib %INST_TEMP_DIR%\fullLib.wixlib %INST_TEMP_DIR%\fullLib_dll%NAME_ADDON%.wixlib %INST_TEMP_DIR%\unittests.wixlib %INST_TEMP_DIR%\tcl.wixlib %INST_TEMP_DIR%\tcl_dll%NAME_ADDON%.wixlib %INST_TEMP_DIR%\sampleScripts.wixlib %INST_TEMP_DIR%\htmldoc.wixlib -o build\%PYTHONSCRIPTVERSION%\PythonScript_%PYTHONSCRIPTVERSION%%NAME_ADDON%.msi -ext "WixToolset.UI.wixext"
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

