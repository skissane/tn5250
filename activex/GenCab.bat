@echo off
setlocal

REM Change to your email address or set the environment variable:
if "%EmailAddress%"=="" echo Set the variable EmailAddress to your email address!!!!
if "%EmailAddress%"=="" set EmailAddress=Unknown@unknown.com


REM The OpenSSL Path to include the DLLs in the CAB file
set OPENSSL=..\..\openssl\dll
REM If NOT using SSL, set this empty (saves some kb)
set OPENSSLFILES=
if /I "%2"=="dll" set OPENSSLFILES=%OPENSSL%\libeay32.dll %OPENSSL%\ssleay32.dll
REM ELSE Specify which of them.

REM If using MSVCRT.DLL
set MSVCRTDLL=
if /I "%2"=="crt" set MSVCRTDLL=%systemroot%\system32\msvcr71.dll %systemroot%\system32\msvcp71.dll

set TargetP=%1
if "%TargetP%" == "" set TargetP=Release

REM Generate the Cab file
cabarc.exe -s 6144 -m LZX:21 n %TargetP%\a5250.cab %TargetP%\A5250.dll a5250.inf %MSVCRTDLL% %OPENSSLFILES%
echo cabarc.exe -s 6144 -m LZX:21 n %TargetP%\a5250.cab %TargetP%\A5250.dll a5250.inf %MSVCRTDLL% %OPENSSLFILES%

REM Please don't sign with my email address :)
if not exist test.pvk makecert -n CN=%EmailAddress% -sv test.PVK test.cer
if not exist test.spc cert2spc test.cer test.spc
signcode /spc test.spc /v test.pvk %TargetP%\a5250.cab

:: Create the ZIP file if in Release
if not /I "%TargetP%"=="Release" goto :END
if not exist "c:\Program Files\7-Zip\7z.exe" goto CANTZIP
:: Do this in two pass so the directory name isn't preserved
"c:\Program Files\7-Zip\7z.exe" a -tzip %TargetP%\A5250.zip ReadMe.txt Term5250.htm
cd %TargetP%
"c:\Program Files\7-Zip\7z.exe" a -tzip A5250.zip A5250.Cab
cd ..
goto :END

:CANTZIP
echo Can't create a zip if 7-Zip isn't installed in it's default directory.

:END
endlocal
