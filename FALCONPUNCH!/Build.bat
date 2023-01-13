@echo off

set includeFolders[0]=source/include
set libraryFolders[0]=source/library
set copyFiles[0]=assets
set rootcd=%cd%
set include=
set libraryF=
set libraries=-lmingw32 -lsfml-system  
set cFiles=

echo Starting compilation...

if not exist bin (mkdir bin)

for /F "tokens=2 delims==" %%s in ('set includeFolders[') do (
    call set "include=%include%-I%%s "
)

for /F "tokens=2 delims==" %%s in ('set libraryFolders[') do (
    call set "libraryF=%libraryF%-L%%s "
    cd %%s

    for /f "tokens=*" %%i in ('dir *.a /b /o-n') do (
        set "should_append=1"
        setlocal enabledelayedexpansion

        echo %%~nxi | findstr /C:"-s">nul && (
            set "should_append=0"
        )

        echo %%~nxi | findstr /C:"-d">nul && (
            set "should_append=0"
        )

        if !should_append! == 1 (
            endlocal
            call set "libraries=%%libraries%%-l%%~nxi "
        )
        endlocal
    )

    cd %rootcd%
)

for /R %%i in (*.cpp) do (
    call set cFiles=%%cFiles%%"%%i" 
)

for /R %%i in (*.dll) do (
    copy %%~nxi bin\%%~nxi
)

for /F "tokens=2 delims==" %%s in ('set copyFiles[') do (
    robocopy %%s bin/%%s  > output.log
    del output.log
)

set "libraries=%libraries:lib=%"
set "libraries=%libraries:.a=%"


ccache g++ -std=c++17 %include%%libraryF%%cFiles%-o "bin/Main.exe" %libraries% 


if %errorlevel% == 0 "bin/Main.exe"