@echo off
echo ========================================
echo  GameEngine Build Script
echo ========================================
echo.

:: Generate project files
echo [1/3] Generating Visual Studio 2022 project files...
call vendor\bin\premake\premake5.exe vs2022
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Premake failed!
    pause
    exit /b 1
)
echo.

:: Build the solution
echo [2/3] Building Engine + Sandbox (Debug x64)...
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Engine.sln /p:Configuration=Debug /p:Platform=x64 /m /v:minimal
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ========================================
    echo  BUILD FAILED
    echo ========================================
    pause
    exit /b 1
)
echo.

:: Copy assets
echo [3/3] Copying assets to output...
xcopy /E /Y /I "Sandbox\assets" "bin\Debug-windows-x86_64\Sandbox\assets" >nul 2>&1

echo.
echo ========================================
echo  BUILD SUCCEEDED
echo  Output: bin\Debug-windows-x86_64\Sandbox\Sandbox.exe
echo ========================================
pause
