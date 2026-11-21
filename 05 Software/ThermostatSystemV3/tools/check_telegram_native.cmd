@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b %errorlevel%
cd /d "%~dp0.."
if not exist artifacts\tests mkdir artifacts\tests
cl /nologo /EHsc /std:c++17 /utf-8 /Itests/telegram_native /Ithermostat/src tests/telegram_access.cpp /Foartifacts/tests/telegram_access.obj /Feartifacts/tests/telegram_access.exe
if errorlevel 1 exit /b %errorlevel%
artifacts\tests\telegram_access.exe
