:: Generate all .zip files
@echo off


SET ORIGINALDIR=%CD%

CD /d %~dp0

SET INSTALLERDIR=%CD%

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

IF EXIST "c:\program files\7-Zip\7z.exe" (
	SET SEVENZIPEXE="c:\program files\7-Zip\7z.exe"
) ELSE (
	SET SEVENZIPEXE=7z.exe
)

mkdir %INST_TEMP_DIR%

%SEVENZIPEXE% 2>&1 > %INST_TEMP_DIR%\junk.txt

IF NOT [%ERRORLEVEL%]==[0] (
	echo Your 7z.exe doesn't seem to be in your PATH or in c:\program files\7-Zip, either add it to your path or install it
	goto error
	)






%PYTHONBUILDDIR%\python extractVersion.py > %INST_TEMP_DIR%\version.txt
SET /p PYTHONSCRIPTVERSION= < %INST_TEMP_DIR%\version.txt

echo Building Release for version %PYTHONSCRIPTVERSION%

echo.
echo Clearing old release directory
rd /s /q %INST_TEMP_DIR%\release

echo Creating directories
mkdir %INST_TEMP_DIR%\release\Full\plugins\PythonScript\lib
mkdir %INST_TEMP_DIR%\release\Full\plugins\PythonScript\scripts
mkdir %INST_TEMP_DIR%\release\Full\plugins\PythonScript\doc
mkdir %INST_TEMP_DIR%\release\Min\plugins\PythonScript\lib
mkdir %INST_TEMP_DIR%\release\Min\plugins\PythonScript\scripts
mkdir %INST_TEMP_DIR%\release\Min\plugins\PythonScript\doc
mkdir %INST_TEMP_DIR%\release\Extra\plugins\PythonScript\lib
mkdir %INST_TEMP_DIR%\release\Tcl\plugins\PythonScript\lib

echo Copying Python27.dll
copy %PYTHONBUILDDIR%\python27.dll %INST_TEMP_DIR%\release\Full\plugins\PythonScript
copy %PYTHONBUILDDIR%\python27.dll %INST_TEMP_DIR%\release\Min\plugins\PythonScript

echo Copying PythonScript.dll
copy ..\%PYTHONSCRIPTDLLDIR%\release\PythonScript.dll %INST_TEMP_DIR%\release\Full\plugins\PythonScript
copy ..\%PYTHONSCRIPTDLLDIR%\release\PythonScript.dll %INST_TEMP_DIR%\release\Min\plugins\PythonScript

echo Copying Help
xcopy /s /q ..\docs\build\html\*.* %INST_TEMP_DIR%\release\Full\plugins\PythonScript\doc
xcopy /s /q ..\docs\build\html\*.* %INST_TEMP_DIR%\release\Min\plugins\PythonScript\doc

echo Copying Scripts
xcopy /s /q ..\scripts\*.* %INST_TEMP_DIR%\release\Full\plugins\PythonScript\scripts
copy ..\scripts\startup.py %INST_TEMP_DIR%\release\Min\plugins\PythonScript\scripts

echo Copying Lib directories
xcopy /s /q ..\PythonLib\full\*.* %INST_TEMP_DIR%\release\Full\plugins\PythonScript\lib
xcopy /s /q ..\PythonLib\full_dll%NAME_ADDON%\*.* %INST_TEMP_DIR%\release\Full\plugins\PythonScript\lib
xcopy /s /q ..\PythonLib\min\*.* %INST_TEMP_DIR%\release\Min\plugins\PythonScript\lib

echo Copying Extra lib directory
xcopy /s /q ..\PythonLib\extra\*.* %INST_TEMP_DIR%\release\Extra\plugins\pythonscript\lib
xcopy /s /q ..\PythonLib\extra_dll%NAME_ADDON%\*.* %INST_TEMP_DIR%\release\Extra\plugins\pythonscript\lib

echo Copying Tcl directory
xcopy /s /q ..\PythonLib\tcl\*.* %INST_TEMP_DIR%\release\tcl\plugins\pythonscript\lib
xcopy /s /q ..\PythonLib\tcl_dll%NAME_ADDON%\*.* %INST_TEMP_DIR%\release\tcl\plugins\pythonscript\lib

mkdir %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%

cd %INST_TEMP_DIR%\release\Full\plugins\PythonScript
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%_PluginAdmin.zip .

cd %INST_TEMP_DIR%\release\Full
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip .

cd %INSTALLERDIR%\%INST_TEMP_DIR%\release\Min
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip .

cd %INSTALLERDIR%\%INST_TEMP_DIR%\release\Extra
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip .

cd %INSTALLERDIR%\%INST_TEMP_DIR%\release\Tcl
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip .

%SEVENZIPEXE% a -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_PDB_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z %INSTALLERDIR%\..\%PYTHONSCRIPTDLLDIR%\release\PythonScript.pdb 

echo Updating local download webpage
echo ^<!--#include virtual="header.inc" --^> > %INSTALLERDIR%\..\www\localdl.shtml
echo ^<h3^>Local Download^</h3^> >> %INSTALLERDIR%\..\www\localdl.shtml
echo Here are the local server links if you can't get to ^<a href="http://sourceforge.net/projects/npppythonscript/files"^>the main one at sourceforge^</a^>.   >> %INSTALLERDIR%\..\www\localdl.shtml
echo Please use the sourceforge link if you can - I have limited bandwidth limits. >> %INSTALLERDIR%\..\www\localdl.shtml
echo ^<br/^>^<br/^>^<br/^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_%PYTHONSCRIPTVERSION%%NAME_ADDON%.msi > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt

echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_%PYTHONSCRIPTVERSION%%NAME_ADDON%.msi"^>Python Script %PYTHONSCRIPTVERSION% Installer (includes all extra files) (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z"^>Python Script %PYTHONSCRIPTVERSION% FULL 7zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Full_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip"^>Python Script %PYTHONSCRIPTVERSION% FULL zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Min_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z"^>Python Script %PYTHONSCRIPTVERSION% MINIMUM 7zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Min_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip"^>Python Script %PYTHONSCRIPTVERSION% MINIMUM zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z"^>Extra libraries 7z (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip"^>Extra libraries zip (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_TclTk_%PYTHONSCRIPTVERSION%%NAME_ADDON%.7z"^>Tcl/Tk libraries 7z (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip > %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\%INST_TEMP_DIR%\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_TclTk_%PYTHONSCRIPTVERSION%%NAME_ADDON%.zip"^>Tcl/Tk libraries zip (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

echo ^<!--#include virtual="footer.inc" --^>  >> %INSTALLERDIR%\..\www\localdl.shtml

echo Updating "Latest Version" in header.inc
%PYTHONBUILDDIR%\python %INSTALLERDIR%\updateLatestVersion.py %INSTALLERDIR%\..\www\header.inc %PYTHONSCRIPTVERSION%


CD /d %ORIGINALDIR%