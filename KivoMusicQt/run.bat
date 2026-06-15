@echo off
setlocal
set "PROJECT_ROOT=%~dp0"
set "BUILD_DIR=%PROJECT_ROOT%build\Release"
set "QT_DIR=%PROJECT_ROOT%.qt\6.8.3\msvc2022_64"
set "EXE_PATH=%BUILD_DIR%\KivoMusicQt.exe"

echo ========================================
echo Kivo Music Qt - Test Launch
echo ========================================
echo.

if not exist "%EXE_PATH%" (
  echo Executable not found: %EXE_PATH%
  exit /b 1
)

set "PATH=%QT_DIR%\bin;%PATH%"
set "QT_PLUGIN_PATH=%QT_DIR%\plugins"
set "QML_IMPORT_PATH=%QT_DIR%\qml"
set "QML2_IMPORT_PATH=%QT_DIR%\qml"

echo Build directory: %BUILD_DIR%
echo Qt directory: %QT_DIR%
echo.
echo Launching KivoMusicQt.exe...
echo ========================================
echo.
"%EXE_PATH%"
exit /b %ERRORLEVEL%
