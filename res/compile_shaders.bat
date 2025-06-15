@echo off
setlocal enabledelayedexpansion

:: Paths (relative to script)
set SHADER_SRC=.\res\shaders
set SHADER_OUT=.\app\src\main\assets\shaders

:: List of shader names (no extension)
set SHADERS=Flat Solid Text Texture

:: Loop over each shader and compile both vertex and fragment shaders
for %%S in (%SHADERS%) do (
    echo Compiling %%S.vert.glsl ...
    glslc -fshader-stage=vert "%SHADER_SRC%\%%S.vert.glsl" -g -o "%SHADER_OUT%\%%S.vert.spv"
    
    echo Compiling %%S.frag.glsl ...
    glslc -fshader-stage=frag "%SHADER_SRC%\%%S.frag.glsl" -g -o "%SHADER_OUT%\%%S.frag.spv"
)

echo.
echo Shader compilation complete.
