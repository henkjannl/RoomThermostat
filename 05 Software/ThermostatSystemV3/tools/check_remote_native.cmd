@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "%~dp0.."
cl /nologo /EHsc /std:c++17 /Itests/native /Iremote-cyd/include /Ishared/protocol/include /Ishared/assets/include tests/remote_controls.cpp /Foartifacts/remote-ui/remote_controls.obj /Feartifacts/remote-ui/remote_controls.exe
if errorlevel 1 exit /b %errorlevel%
artifacts\remote-ui\remote_controls.exe artifacts/remote-ui
