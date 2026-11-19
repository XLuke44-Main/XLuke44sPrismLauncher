@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..") do set "PROJECT_ROOT=%%~fI"
set "RESULT=0"

if not exist "%PROJECT_ROOT%\CMakeLists.txt" (
    echo ERROR: Could not locate the project root.
    set "RESULT=1"
    goto :failure
)

for %%D in ("%PROJECT_ROOT%\build-windows" "%PROJECT_ROOT%\build-windows-portable" "%PROJECT_ROOT%\dist") do (
    if exist "%%~fD\" (
        rmdir /s /q "%%~fD"
        if errorlevel 1 (
            echo ERROR: Could not remove generated Windows directory: %%~fD
            set "RESULT=1"
            goto :failure
        )
        if exist "%%~fD\" (
            echo ERROR: Could not remove generated Windows directory: %%~fD
            set "RESULT=1"
            goto :failure
        )
    )
)

for %%F in ("%PROJECT_ROOT%\XLuke44s-Prismlauncher-*-windows-portable.zip") do (
    if exist "%%~fF" (
        del /f /q "%%~fF"
        if errorlevel 1 (
            echo ERROR: Could not remove generated Windows archive: %%~fF
            set "RESULT=1"
            goto :failure
        )
        if exist "%%~fF" (
            echo ERROR: Could not remove generated Windows archive: %%~fF
            set "RESULT=1"
            goto :failure
        )
    )
)

endlocal & exit /b 0

:failure
if "%RESULT%"=="0" set "RESULT=1"
echo Windows portable cleanup failed.
endlocal & exit /b %RESULT%
