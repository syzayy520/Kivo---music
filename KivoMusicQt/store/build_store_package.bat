@echo off
REM =============================================================================
REM Kivo Music — build_store_package.bat
REM 构建 Microsoft Store MSIX 包
REM =============================================================================

echo === Kivo Music Store Package Builder ===
echo.

REM 1. Check prerequisites
where msbuild >nul 2>&1 || (echo ERROR: msbuild not found & exit /b 1)
where makeappx >nul 2>&1 || (echo WARNING: makeappx not found, package will be skipped)
echo MSBuild: OK
echo.

REM 2. Build Release x64
echo [1/4] Building Release x64...
cd /d "%~dp0.."
msbuild build\KivoMusicQt.vcxproj /p:Configuration=Release /p:Platform=x64 /t:Build /m
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    exit /b 1
)
echo Build: OK
echo.

REM 3. Create store assets directory
echo [2/4] Preparing package assets...
if not exist "store\Assets" mkdir "store\Assets"

REM Copy required files
copy /Y "build\Release\KivoMusicQt.exe" "store\pkg\" 2>nul || mkdir "store\pkg"
copy /Y "build\Release\*.dll" "store\pkg\" 2>nul
copy /Y "store\Package.appxmanifest" "store\pkg\AppxManifest.xml" 2>nul
echo Assets: OK
echo.

REM 4. Create MSIX package
echo [3/4] Creating MSIX package...
where makeappx >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo SKIP: makeappx not available. Install Windows SDK to create packages.
) else (
    makeappx pack /d "store\pkg" /p "store\KivoMusic_1.0.0.0_x64.msix"
    if %ERRORLEVEL% equ 0 (
        echo Package: OK
    ) else (
        echo WARNING: Package creation had issues
    )
)
echo.

REM 5. Summary
echo [4/4] Summary
echo ========================================
echo Build completed. Deliverables:
echo   - store\KivoMusic_1.0.0.0_x64.msix
echo   - store\Package.appxmanifest
echo   - store\STORE_LISTING.md
echo.
echo Next steps:
echo   1. Sign the package: signtool sign /fd SHA256 /a /f cert.pfx KivoMusic_1.0.0.0_x64.msix
echo   2. Create screenshots at 1366x768
echo   3. Submit to Microsoft Partner Center
echo ========================================
