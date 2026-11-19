@echo off
setlocal EnableExtensions DisableDelayedExpansion

set "QT_ROOT=C:\Qt\5.15.2\msvc2019_64"
set "JAVA_HOME=C:\Program Files\Java\jdk1.8.0_51"
set "OPENSSL_ROOT=C:\Program Files\OpenSSL-Win64"






for %%I in ("%~dp0..") do set "PROJECT_ROOT=%%~fI"
set "BUILD_DIR=%PROJECT_ROOT%\build-windows-portable"
set "DIST_ROOT=%PROJECT_ROOT%\dist"
set "DIST_DIR=%DIST_ROOT%\windows-portable"
set "APP_BINARY_NAME=xluke44-prismlauncher"
if not defined LAUNCHER_QT_VERSION_MAJOR set "LAUNCHER_QT_VERSION_MAJOR=5"

if not exist "%PROJECT_ROOT%\CMakeLists.txt" goto :missing_project
if not "%LAUNCHER_QT_VERSION_MAJOR%"=="5" if not "%LAUNCHER_QT_VERSION_MAJOR%"=="6" goto :invalid_qt_version


if defined JAVA_HOME (
    if not exist "%JAVA_HOME%\bin\java.exe" goto :missing_java
    if not exist "%JAVA_HOME%\bin\javac.exe" goto :missing_java
    if not exist "%JAVA_HOME%\bin\jar.exe" goto :missing_java
    set "PATH=%JAVA_HOME%\bin;%PATH%"
)

where cmake >nul 2>&1
if errorlevel 1 goto :missing_cmake
where java >nul 2>&1
if errorlevel 1 goto :missing_java
where javac >nul 2>&1
if errorlevel 1 goto :missing_java
where jar >nul 2>&1
if errorlevel 1 goto :missing_java

set "CMAKE_VERSION="
for /f "tokens=3" %%V in ('cmake --version 2^>^&1') do if not defined CMAKE_VERSION set "CMAKE_VERSION=%%V"
if not "%CMAKE_VERSION%"=="3.31.12" goto :unsupported_cmake

set "JAVA_VERSION="
for /f "tokens=2" %%V in ('javac -version 2^>^&1') do set "JAVA_VERSION=%%V"
if not defined JAVA_VERSION goto :missing_java
if /i not "%JAVA_VERSION:~0,3%"=="1.8" goto :unsupported_java

set "PROJECT_VERSION="
for /f "tokens=3" %%V in ('findstr /b /c:"project(Launcher VERSION " "%PROJECT_ROOT%\CMakeLists.txt"') do set "PROJECT_VERSION=%%V"
set "PROJECT_VERSION=%PROJECT_VERSION:)=%"
if not "%PROJECT_VERSION%"=="1.0.5.0" goto :invalid_version


if not defined QT_ROOT if "%LAUNCHER_QT_VERSION_MAJOR%"=="5" set "QT_ROOT=C:\Qt\5.15.2\msvc2019_64"
if not defined QT_ROOT goto :missing_qt_root
for %%I in ("%QT_ROOT%") do set "QT_ROOT=%%~fI"



if not defined OPENSSL_ROOT (
    for %%I in ("%QT_ROOT%\..\..\Tools\OpenSSL\Win_x64") do if exist "%%~fI\bin\libcrypto-1_1*.dll" set "OPENSSL_ROOT=%%~fI"
)

set "QT_PACKAGE_DIR=%QT_ROOT%\lib\cmake\Qt%LAUNCHER_QT_VERSION_MAJOR%"
set "QT_PACKAGE_CONFIG=%QT_PACKAGE_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Config.cmake"
set "WINDEPLOYQT_EXE=%QT_ROOT%\bin\windeployqt.exe"
if not exist "%QT_PACKAGE_CONFIG%" goto :missing_qt
if not exist "%WINDEPLOYQT_EXE%" goto :missing_windeployqt
set "PATH=%QT_ROOT%\bin;%PATH%"



where dumpbin >nul 2>&1
if errorlevel 1 call :initialize_msvc
where dumpbin >nul 2>&1
if errorlevel 1 goto :missing_dumpbin
if not defined VCINSTALLDIR call :initialize_msvc
if not defined VCINSTALLDIR goto :missing_msvc

set "PACKAGE_FILE=%PROJECT_ROOT%\XLuke44s-Prismlauncher-%PROJECT_VERSION%-windows-portable.zip"

 echo.
echo ============================================================
echo  XLuke44 PrismLauncher %PROJECT_VERSION% - Windows portable build
echo ============================================================
echo  CMake:       %CMAKE_VERSION%
echo  Java:        %JAVA_VERSION%
echo  Qt:          %QT_ROOT%
echo  Source:      "%PROJECT_ROOT%"
echo  Build:       "%BUILD_DIR%"
echo  Distribution:"%DIST_DIR%"
echo.


if exist "%DIST_DIR%\" (
    rmdir /s /q "%DIST_DIR%"
    if errorlevel 1 goto :failure
    if exist "%DIST_DIR%\" goto :failure
)
if exist "%BUILD_DIR%\" (
    rmdir /s /q "%BUILD_DIR%"
    if errorlevel 1 goto :failure
    if exist "%BUILD_DIR%\" goto :failure
)
if exist "%PACKAGE_FILE%" (
    del /f /q "%PACKAGE_FILE%"
    if errorlevel 1 goto :failure
    if exist "%PACKAGE_FILE%" goto :failure
)
if not exist "%DIST_ROOT%\" (
    mkdir "%DIST_ROOT%"
    if errorlevel 1 goto :failure
)

echo Configuring CMake...
cmake -S "%PROJECT_ROOT%\." -B "%BUILD_DIR%" ^
    "-DCMAKE_BUILD_TYPE=Release" ^
    "-DCMAKE_INSTALL_PREFIX=%DIST_DIR%" ^
    "-DLauncher_QT_VERSION_MAJOR=%LAUNCHER_QT_VERSION_MAJOR%" ^
    "-DCMAKE_PREFIX_PATH=%QT_ROOT%" ^
    "-DQt%LAUNCHER_QT_VERSION_MAJOR%_DIR=%QT_PACKAGE_DIR%" ^
    "-DLauncher_WINDEPLOYQT_EXECUTABLE=%WINDEPLOYQT_EXE%" ^
    "-DLauncher_OPENSSL_ROOT=%OPENSSL_ROOT%" ^
    "-DCMAKE_GP_TOOL=dumpbin" ^
    "-DLauncher_BUILD_PLATFORM=windows-portable" ^
    "-DLauncher_APP_BINARY_NAME=%APP_BINARY_NAME%" ^
    "-DLauncher_FORCE_BUNDLED_LIBS=ON" ^
    "-DINSTALL_BUNDLE=full" ^
    "-DQUAZIP_FETCH_LIBS=OFF"
if errorlevel 1 goto :failure

echo.
echo Building both Windows executables and Java archives...
cmake --build "%BUILD_DIR%" --config Release --parallel
if errorlevel 1 goto :failure

echo.
echo Installing the complete Runtime component (executables, Qt runtime, plugins and JARs)...
cmake --install "%BUILD_DIR%" --config Release --component Runtime
if errorlevel 1 goto :failure

echo.
echo Installing portable-mode marker...
cmake --install "%BUILD_DIR%" --config Release --component portable
if errorlevel 1 goto :failure




for %%F in (
    "%DIST_DIR%\%APP_BINARY_NAME%.exe"
    "%DIST_DIR%\%APP_BINARY_NAME%_filelink.exe"
    "%DIST_DIR%\portable.txt"
    "%DIST_DIR%\qt.conf"
    "%DIST_DIR%\qtlogging.ini"
    "%DIST_DIR%\jars\NewLaunch.jar"
    "%DIST_DIR%\jars\NewLaunchLegacy.jar"
    "%DIST_DIR%\jars\JavaCheck.jar"
    "%DIST_DIR%\plugins\platforms\qwindows.dll"
    "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Core.dll"
    "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Gui.dll"
    "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Network.dll"
    "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Widgets.dll"
    "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Xml.dll"
    "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Concurrent.dll"
) do (
    call :require_nonempty "%%~F"
    if errorlevel 1 goto :incomplete_distribution
)

if "%LAUNCHER_QT_VERSION_MAJOR%"=="6" (
    call :require_nonempty "%DIST_DIR%\Qt6Core5Compat.dll"
    if errorlevel 1 goto :incomplete_distribution
)
if exist "%DIST_DIR%\plugins\iconengines\qsvgicon.dll" (
    call :require_nonempty "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Svg.dll"
    if errorlevel 1 goto :incomplete_distribution
)
if exist "%DIST_DIR%\plugins\imageformats\qsvg.dll" (
    call :require_nonempty "%DIST_DIR%\Qt%LAUNCHER_QT_VERSION_MAJOR%Svg.dll"
    if errorlevel 1 goto :incomplete_distribution
)

if "%LAUNCHER_QT_VERSION_MAJOR%"=="5" (
    set "OPENSSL_CRYPTO_PRESENT="
    set "OPENSSL_SSL_PRESENT="
    for %%F in ("%DIST_DIR%\libcrypto-1_1*.dll") do if exist "%%~fF" set "OPENSSL_CRYPTO_PRESENT=1"
    for %%F in ("%DIST_DIR%\libssl-1_1*.dll") do if exist "%%~fF" set "OPENSSL_SSL_PRESENT=1"
    if not defined OPENSSL_CRYPTO_PRESENT (
        echo ERROR: Qt 5 OpenSSL crypto runtime DLL is missing from the portable root.
        goto :incomplete_distribution
    )
    if not defined OPENSSL_SSL_PRESENT (
        echo ERROR: Qt 5 OpenSSL SSL runtime DLL is missing from the portable root.
        goto :incomplete_distribution
    )
)

call :verify_portable_dlls "%DIST_DIR%"
if errorlevel 1 goto :incomplete_distribution

findstr /c:"Plugins=plugins" "%DIST_DIR%\qt.conf" >nul
if errorlevel 1 (
    echo ERROR: qt.conf does not point Qt to the portable plugins directory.
    goto :incomplete_distribution
)


for /r "%DIST_DIR%" %%F in (*.pdb *.ilk *.exp *.lib *.obj *.o *.a *.map *.tmp) do (
    if exist "%%~fF" (
        del /f /q "%%~fF"
        if errorlevel 1 goto :failure
        if exist "%%~fF" goto :failure
    )
)

echo.
echo Creating portable ZIP package...
cmake -E chdir "%DIST_ROOT%" cmake -E tar cf "%PACKAGE_FILE%" --format=zip windows-portable
if errorlevel 1 goto :failure
if not exist "%PACKAGE_FILE%" goto :failure




cmake -E tar tf "%PACKAGE_FILE%" >nul
if errorlevel 1 (
    echo ERROR: The generated ZIP cannot be read.
    goto :failure
)

for %%F in ("%PACKAGE_FILE%") do if %%~zF EQU 0 (
    echo ERROR: The generated ZIP is empty.
    goto :failure
)

echo.
echo ============================================================
echo  Windows portable build completed successfully!
echo  Package: "%PACKAGE_FILE%"
echo  ZIP readability: passed
echo  Staged runtime checks: passed
echo ============================================================
endlocal & exit /b 0

:require_nonempty
if not exist "%~1" (
    echo ERROR: Required portable file is missing: "%~1"
    exit /b 1
)
for %%Z in ("%~1") do if %%~zZ EQU 0 (
    echo ERROR: Required portable file is empty: "%~1"
    exit /b 1
)
exit /b 0

:verify_portable_dlls
setlocal EnableExtensions EnableDelayedExpansion
set "PORTABLE_ROOT=%~1"
for %%F in ("%~1\*.dll") do if exist "%%~fF" (
    if %%~zF EQU 0 (
        echo ERROR: Empty portable runtime DLL: "%%~fF"
        endlocal & exit /b 1
    )
)
if exist "%~1\plugins\" (
    for /r "%~1\plugins" %%F in (*.dll) do (
        if %%~zF EQU 0 (
            echo ERROR: Empty Qt plugin: "%%~fF"
            endlocal & exit /b 1
        )
    )
)
endlocal & exit /b 0

:missing_project
echo ERROR: CMakeLists.txt was not found in the parent directory of scripts\.
goto :failure
:missing_cmake
echo ERROR: CMake is not installed or is not available on PATH.
goto :failure
:unsupported_cmake
echo ERROR: This script requires CMake 3.31.12. Detected: %CMAKE_VERSION%
goto :failure
:missing_java
echo ERROR: A JDK 8 installation with java.exe, javac.exe and jar.exe is required.
echo Set JAVA_HOME to the JDK 8 installation.
goto :failure
:unsupported_java
echo ERROR: This project requires JDK 8. Detected javac version: %JAVA_VERSION%
goto :failure
:invalid_version
echo ERROR: The supplied project version must remain exactly 1.0.5.0. Detected: %PROJECT_VERSION%
goto :failure
:invalid_qt_version
echo ERROR: LAUNCHER_QT_VERSION_MAJOR must be 5 or 6.
goto :failure
:missing_qt_root
echo ERROR: QT_ROOT is not set and no supported default Qt prefix is configured.
echo Set QT_ROOT to the root of the selected Windows desktop Qt kit.
goto :failure
:missing_qt
echo ERROR: Qt%LAUNCHER_QT_VERSION_MAJOR% CMake package was not found: "%QT_PACKAGE_CONFIG%"
goto :failure
:missing_windeployqt
echo ERROR: windeployqt.exe was not found in the selected Qt kit: "%WINDEPLOYQT_EXE%"
goto :failure
:missing_msvc
echo ERROR: Visual Studio 2022 x64 C++ Build Tools could not be initialized.
goto :failure
:missing_dumpbin
echo ERROR: dumpbin.exe is required so BundleUtilities can analyze PE dependencies.
echo Install the VS 2022 MSVC x64/x64 tools workload and run this build again.
goto :failure
:incomplete_distribution
echo ERROR: The staged portable distribution is incomplete; no ZIP will be retained.
goto :failure
:failure
echo.
echo Windows portable build failed.
if exist "%PACKAGE_FILE%" del /f /q "%PACKAGE_FILE%"
endlocal & exit /b 1

:initialize_msvc


set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" set "VSWHERE=%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" exit /b 0
set "VS_INSTALL="
set "VS_PATH_FILE=%TEMP%\xluke44-vswhere-%RANDOM%-%RANDOM%.txt"
"%VSWHERE%" -latest -version "[17.0,18.0)" -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VS_PATH_FILE%" 2>nul
if not errorlevel 1 if exist "%VS_PATH_FILE%" set /p "VS_INSTALL=" < "%VS_PATH_FILE%"
if exist "%VS_PATH_FILE%" del /f /q "%VS_PATH_FILE%" >nul 2>&1
if not defined VS_INSTALL exit /b 0
set "VS_VCVARS=%VS_INSTALL%\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VS_VCVARS%" exit /b 0
call "%VS_VCVARS%"
exit /b 0
