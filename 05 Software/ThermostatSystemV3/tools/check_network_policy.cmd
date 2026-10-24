@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "%~dp0.."
if not exist artifacts\network-tests mkdir artifacts\network-tests
cl /nologo /EHsc /std:c++17 /Ithermostat/src tests/network_policy.cpp /Foartifacts/network-tests/network_policy.obj /Feartifacts/network-tests/network_policy.exe
if errorlevel 1 exit /b %errorlevel%
artifacts\network-tests\network_policy.exe
