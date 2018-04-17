:: Run this script to put all the release packages together.
:: You should have a built PythonScript in release mode, a built python, and all the libraries checked out

call buildInstaller.bat %1
call buildReleaseZips.bat %1
