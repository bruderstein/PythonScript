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

mkdir temp

%SEVENZIPEXE% 2>&1 > temp\junk.txt

IF NOT [%ERRORLEVEL%]==[0] (
	echo Your 7z.exe doesn't seem to be in your PATH or in c:\program files\7-Zip, either add it to your path or install it
	goto error
	)






%PYTHONBUILDDIR%\python extractVersion.py > temp\version.txt
SET /p PYTHONSCRIPTVERSION= < temp\version.txt

echo Building Release for version %PYTHONSCRIPTVERSION%

echo.
echo Clearing old release directory
rd /s /q temp\release

echo Creating directories
mkdir temp\release\Full\plugins\PythonScript\lib
mkdir temp\release\Full\plugins\PythonScript\scripts
mkdir temp\release\Full\plugins\doc\PythonScript
mkdir temp\release\Min\plugins\PythonScript\lib
mkdir temp\release\Min\plugins\PythonScript\scripts
mkdir temp\release\Min\plugins\doc\PythonScript
mkdir temp\release\Extra\plugins\PythonScript\lib
mkdir temp\release\Tcl\plugins\PythonScript\lib\tcl

echo Copying Python27.dll
copy %PYTHONBUILDDIR%\python27.dll temp\release\Full
copy %PYTHONBUILDDIR%\python27.dll temp\release\Min

echo Copying PythonScript.dll
copy ..\bin\Release\PythonScript.dll temp\release\Full\plugins
copy ..\bin\release\PythonScript.dll temp\release\min\plugins

echo Copying Help
copy ..\docs\build\htmlhelp\PythonScript.chm temp\release\full\plugins\doc\PythonScript
copy ..\docs\build\htmlhelp\PythonScript.chm temp\release\min\plugins\doc\PythonScript

echo Copying Scripts
xcopy /s /q ..\scripts\*.* temp\release\full\plugins\PythonScript\scripts
copy ..\scripts\startup.py temp\release\min\plugins\PythonScript\scripts

echo Copying Lib directories
xcopy /s /q ..\PythonLib\full\*.* temp\release\full\plugins\PythonScript\lib
xcopy /s /q ..\PythonLib\min\*.* temp\release\min\plugins\PythonScript\lib

echo Copying Extra lib directory
xcopy /s /q ..\PythonLib\Extra\*.* temp\release\extra\plugins\pythonscript\lib

echo Copying Tcl directory
xcopy /s /q ..\PythonLib\tcl\*.* temp\release\tcl\plugins\pythonscript\lib\tcl



cd temp\release\Full
mkdir %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%.zip .

cd %INSTALLERDIR%\temp\release\Min
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%.zip .

cd %INSTALLERDIR%\temp\release\Extra
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.zip .

cd %INSTALLERDIR%\temp\release\Tcl
%SEVENZIPEXE% a -r -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%.7z .
%SEVENZIPEXE% a -r -tzip %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%.zip .

%SEVENZIPEXE% a -t7z %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_PDB_%PYTHONSCRIPTVERSION%.7z %INSTALLERDIR%\..\bin\release\PythonScript.pdb 

echo Updating local download webpage
echo ^<!--#include virtual="header.inc" --^> > %INSTALLERDIR%\..\www\localdl.shtml
echo ^<h3^>Local Download^</h3^> >> %INSTALLERDIR%\..\www\localdl.shtml
echo Here are the local server links if you can't get to ^<a href="http://sourceforge.net/projects/npppythonscript/files"^>the main one at sourceforge^</a^>.   >> %INSTALLERDIR%\..\www\localdl.shtml
echo Please use the sourceforge link if you can - I have limited bandwidth limits. >> %INSTALLERDIR%\..\www\localdl.shtml
echo ^<br/^>^<br/^>^<br/^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_%PYTHONSCRIPTVERSION%.msi > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt

echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_%PYTHONSCRIPTVERSION%.msi"^>Python Script %PYTHONSCRIPTVERSION% Installer (includes all extra files) (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%.7z > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Full_%PYTHONSCRIPTVERSION%.7z"^>Python Script %PYTHONSCRIPTVERSION% FULL 7zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Full_%PYTHONSCRIPTVERSION%.zip > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Full_%PYTHONSCRIPTVERSION%.zip"^>Python Script %PYTHONSCRIPTVERSION% FULL zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%.7z > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Min_%PYTHONSCRIPTVERSION%.7z"^>Python Script %PYTHONSCRIPTVERSION% MINIMUM 7zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_Min_%PYTHONSCRIPTVERSION%.zip > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_Min_%PYTHONSCRIPTVERSION%.zip"^>Python Script %PYTHONSCRIPTVERSION% MINIMUM zip version (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.7z > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.7z"^>Extra libraries 7z (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.zip > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_ExtraLibs_%PYTHONSCRIPTVERSION%.zip"^>Extra libraries zip (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%.7z > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_TclTk_%PYTHONSCRIPTVERSION%.7z"^>Tcl/Tk libraries 7z (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

%PYTHONBUILDDIR%\python %INSTALLERDIR%\humanReadableSize.py %INSTALLERDIR%\build\%PYTHONSCRIPTVERSION%\PythonScript_TclTk_%PYTHONSCRIPTVERSION%.zip > %INSTALLERDIR%\temp\size.txt
SET /p PYTHONSCRIPT_FILESIZE= < %INSTALLERDIR%\temp\size.txt
echo ^<br/^>^<a href="http://www.brotherstone.co.uk/npp/ps/downloads/%PYTHONSCRIPTVERSION%/PythonScript_TclTk_%PYTHONSCRIPTVERSION%.zip"^>Tcl/Tk libraries zip (%PYTHONSCRIPT_FILESIZE%)^</a^>  >> %INSTALLERDIR%\..\www\localdl.shtml

echo ^<!--#include virtual="footer.inc" --^>  >> %INSTALLERDIR%\..\www\localdl.shtml

echo Updating "Latest Version" in header.inc
%PYTHONBUILDDIR%\python %INSTALLERDIR%\updateLatestVersion.py %INSTALLERDIR%\..\www\header.inc %PYTHONSCRIPTVERSION%


CD /d %ORIGINALDIR%