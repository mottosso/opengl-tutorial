@echo off

:: Prepare build directory
mkdir build
pushd build

cl^
  -Zi^
  -EHsc^
  -nologo^
  -I "../include"^
  -I "../ext/gl3w/include"^
  -I "../ext/imgui/include"^
  -I "../ext/stb"^
  -I "../ext/glm"^
  ../src/shader.cpp^
  ../src/main.cpp^
  ../ext/gl3w/src/gl3w.c^
  ../ext/imgui/src/imgui.cpp^
  ../ext/imgui/src/draw.cpp^
  ../ext/imgui/src/ref/glfw_gl3.cpp^
  glfw3.lib^
  opengl32.lib^
  user32.lib^
  gdi32.lib^
  comdlg32.lib^
  advapi32.lib^
  shell32.lib^
  ole32.lib^
  oleaut32.lib^
  -link^
    /OUT:main.exe^
    /LIBPATH:"../lib"^
    /NODEFAULTLIB:LIBCMT &&^
popd || popd && exit /b 1
