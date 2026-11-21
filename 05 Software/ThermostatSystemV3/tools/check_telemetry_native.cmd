@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "%~dp0.."
if not exist artifacts\telemetry-tests mkdir artifacts\telemetry-tests
cl /nologo /EHsc /std:c++17 /Itests/telemetry_native /Ithermostat/src tests/telemetry.cpp /Foartifacts/telemetry-tests/telemetry.obj /Feartifacts/telemetry-tests/telemetry.exe
if errorlevel 1 exit /b %errorlevel%
artifacts\telemetry-tests\telemetry.exe
