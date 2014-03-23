:: Generate all .zip files
@echo off


SET ORIGINALDIR=%CD%

CD /d %~dp0

SET INSTALLERDIR=%CD%

IF NOT EXIST "buildPaths.bat" (
	echo You need to copy/rename buildPaths.bat.orig to buildPaths.bat, and edit it to your local configuration
	goto error
	)

CALL buildPaths.bat

IF NOT EXIST "%PYTHONBUILDDIR%\pcbuild\python.exe" (
	echo Your PYTHONBUILDDIR in buildPaths.bat does not contain PCBuild\python.exe.  Please set PYTHONBUILDDIR to the root of a built Python 2.7
	goto error
	)

IF NOT EXIST "%PYTHONBUILDDIR%\pcbuild\python27.dll" (
	echo Your PYTHONBUILDDIR in buildPaths.bat does not contain PCBuild\python27.dll.  Please set PYTHONBUILDDIR to the root of a built Python 2.7
	goto error
	)

IF EXIST "c:\program files\7-Zip\7z.exe" (
	SET SEVENZIPEXE="c:\program files\7-Zip\7z.exe"
) ELSE (
	SET SEVENZIPEXE=7z.exe
)

%SEVENZIPEXE% 2>&1 > temp\junk.txt

IF NOT [%ERRORLEVEL%]==[0] (
	echo Your 7z.exe doesn't seem to be in your PATH or in c:\program files\7-Zip, either add it to your path or install it
	goto error
	)






%PYTHONBUILDDIR%\pcbuild\python extractVersion.py > temp\version.txt
SET /p PYTHONSCRIPTVERSION= < temp\version.txt

echo Building Release for version %PYTHONSCRIPTVERSION%

echo.
echo Clearing old release directory
rd /s /q temp\release

echo Creating directories
mkdir temp\release\Full\plugins\PythonScript\lib
mkdir temp\release\Full\plugins\doc\PythonScript
mkdir temp\release\Min\plugins\PythonScript\lib
mkdir temp\release\Min\plugins\doc\PythonScript
mkdir temp\release\Extra\plugins\PythonScript\lib
mkdir temp\release\Tcl\plugins\PythonScript\lib\tcl

echo Copying Python27.dll
copy %PYTHONBUILDDIR%\PCBuild\python27.dll temp\release\Full
copy %PYTHONBUILDDIR%\PCBuild\python27.dll temp\release\Min

echo Copying PythonScript.dll
copy ..\bin\Release\PythonScript.dll temp\release\Full\plugins
copy ..\bin\release\PythonScript.dll temp\release\min\plugins

echo Copying Help
copy ..\docs\build\htmlhelp\PythonScript.chm temp\release\full\plugins\doc\PythonScript
copy ..\docs\build\htmlhelp\PythonScript.chm temp\release\min\plugins\doc\PythonScript

echo Copying Lib directories
xcopy /s /q ..\PythonLib\full\*.* temp\release\full\plugins\PythonScript\lib
xcopy /s /q ..\PythonLib\min\*.* temp\release\min\plugins\PythonScript\lib

echo Copying Extra lib directory
xcopy /s /q ..\PythonLib\Extra\*.* temp\release\extra\plugins\pythonscript\lib

echo Copying Tcl directory
xcopy /s /q ..\PythonLib\tcl\*.* temp\release\tcl\plugins\pythonscript\lib\tcl



cd temp\release\Full
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\PythonScript_Full_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\PythonScript_Full_%PYTHONSCRIPTVERSION%.zip .

cd %INSTALLERDIR%\temp\release\Min
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\PythonScript_Min_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\PythonScript_Min_%PYTHONSCRIPTVERSION%.zip .

cd %INSTALLERDIR%\temp\release\Extra
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.zip .

cd %INSTALLERDIR%\temp\release\Tcl
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\PythonScript_TclTk_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\PythonScript_TclTk_%PYTHONSCRIPTVERSION%.zip .

CD /d %ORIGINALDIR%