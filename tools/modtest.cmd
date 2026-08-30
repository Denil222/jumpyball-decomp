@echo off
setlocal
call "D:\VS\BuildTools\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 1
cd /d "%~dp0"
if not exist build mkdir build
cl /nologo /W4 /O2 /TC /MT /D_CRT_SECURE_NO_WARNINGS /Fobuild\ /Febuild\modtest.exe modtest.c ..\jb_mod.c ..\jb_mod_effects.c
