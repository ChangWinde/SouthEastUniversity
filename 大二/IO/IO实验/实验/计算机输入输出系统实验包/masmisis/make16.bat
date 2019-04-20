@echo off
REM make16.bat,for assembling and linking 16-bit programs (.EXE)
set path=d:\masmisis
set lib=d:\masmisis
D:\masmisis\ml /c /Zd /Zi /Zm /Zf %1
if errorlevel 1 goto terminate
set str=%1
set str=%str:~0,-4%
D:\masmisis\link /DEBUGB  /STACK:1024 %str%.obj,%str%.exe,nul.map,,
if errorlevel 1 goto terminate
DIR %str%.*
@echo on
rem  SUCCESS!
:terminate
@echo on

