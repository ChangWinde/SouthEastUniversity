@echo off
set path=d:\masmisis
set lib=d:\masmisis
D:\masmisis\ml /c /Zd /Zi /Zm /Zf %1
set str=%1
set str=%str:~0,-4%
D:\masmisis\link /CODEVIEW /NOD /DEB /DEBUGB  /STACK:1024 %str%.obj,%str%.exe,nul.map,,