@echo off
setlocal
set VCVARS=D:\VS\BuildTools\VC\Auxiliary\Build\vcvars32.bat
set SDL=D:\vcpkg\installed\x86-windows

if not exist "%VCVARS%" echo missing %VCVARS% && exit /b 1
if not exist "%SDL%\lib\SDL2.lib" echo missing %SDL%\lib\SDL2.lib && exit /b 1

call "%VCVARS%" >nul || exit /b 1
cd /d "%~dp0"
if not exist build mkdir build

cl /nologo /W4 /O2 /TC /MT /D_CRT_SECURE_NO_WARNINGS /I"%SDL%\include\SDL2" /Fobuild\ /Febuild\jumpyball.exe ^
    jb_gfx.c jb_gfx_tile.c jb_bmp.c jb_track.c jb_level.c jb_trackrow_tiled.c jb_trackrow_sky.c jb_trackrow_ice_alt.c jb_trackrow_grass.c jb_trackrow_forest.c jb_ball.c jb_player.c jb_stage.c jb_text.c jb_mod.c jb_mod_effects.c jb_audio.c jb_menu.c jb_keyconfig.c jb_assets.c jb_appassets.c jb_platform_sdl2.c jb_audio_sdl2.c jb_touch_sdl2.c jb_main.c ^
    /link /SUBSYSTEM:WINDOWS /NODEFAULTLIB:msvcrt.lib "%SDL%\lib\SDL2.lib" "%SDL%\lib\manual-link\SDL2main.lib" shell32.lib
if errorlevel 1 exit /b 1

copy /y "%SDL%\bin\SDL2.dll" build\ >nul

if not exist build\BITMAP mkdir build\BITMAP
copy /y BITMAP\*.bmp build\BITMAP\ >nul
if not exist build\Sounds mkdir build\Sounds
copy /y Sounds\*.wav build\Sounds\ >nul
if not exist build\Musics mkdir build\Musics
copy /y Musics\*.tkm build\Musics\ >nul

echo BUILD OK: build\jumpyball.exe
