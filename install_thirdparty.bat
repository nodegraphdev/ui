@ECHO OFF

set A_CDSTART=%cd%


@REM Git Check
git --version
set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto missingGit

@REM CMake Check
cmake --version
set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto missingCMake

echo ======= Installing libraries =======
rmdir /S /Q thirdparty\win
mkdir thirdparty\win
cd thirdparty\win

mkdir lib
mkdir temp
mkdir bin

echo =========== Building GLFW ===========
mkdir temp\glfw
git clone -b 3.3-stable https://github.com/glfw/glfw.git temp\glfw

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto cloneFail

cmake temp\glfw -B temp\glfw_build -D GLFW_BUILD_EXAMPLES=OFF -D GLFW_BUILD_TESTS=OFF -D GLFW_BUILD_DOCS=OFF

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto cmakeFail

cmake --build temp\glfw_build --config Release

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto buildFail

echo =========== Copying GLFW ===========
copy temp\glfw_build\src\Release\glfw3.lib lib\glfw3.lib
robocopy temp\glfw\include include /E

echo ========= Building FreeType =========
mkdir temp\freetype
git clone -b VER-2-11-1 https://gitlab.freedesktop.org/freetype/freetype.git temp\freetype

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto cloneFail

cmake temp\freetype -B temp\freetype_build -D CMAKE_BUILD_TYPE=Release

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto cmakeFail

cmake --build temp\freetype_build --config Release

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto buildFail

echo ========= Copying FreeType =========
copy temp\freetype_build\Release\freetype.lib lib\freetype.lib
robocopy temp\freetype\include include /E


echo ========= Building WinFlexBison =========
mkdir temp\winflexbison
git clone -b "v2.5.25" https://github.com/lexxmark/winflexbison.git temp\winflexbison

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto cloneFail

cmake temp\winflexbison -B temp\winflexbison_build -D CMAKE_BUILD_TYPE=Release

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto cmakeFail

cmake --build temp\winflexbison_build --config Release

set A_ERRORLEVEL=%ERRORLEVEL%
if %A_ERRORLEVEL% neq 0 goto buildFail

echo ========= Copying WinFlexBison =========
robocopy temp\winflexbison\bin\Release bin /E





echo ============ Cleaning up ============
rmdir /S /Q temp

echo ================
echo Install success!
echo ================
cd %A_CDSTART%
exit /b 0

:missingGit
echo =============================
echo Failed to install thirdparty!
echo Please download Git!
echo =============================
goto failEnd

:missingCMake
echo =============================
echo Failed to install thirdparty!
echo Please download CMake!
echo =============================
goto failEnd

:cloneFail
echo =============================
echo Failed to install thirdparty!
echo Could not clone repository!
echo =============================
goto failEnd

:cmakeFail
echo =============================
echo Failed to install thirdparty!
echo Could not create project!
echo =============================
goto failEnd

:buildFail
echo =============================
echo Failed to install thirdparty!
echo Could not build project!
echo =============================
goto failEnd

:failEnd
cd %A_CDSTART%
exit /b 1
