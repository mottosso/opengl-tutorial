@echo off

:: Prepare build directory
pushd build

cl^
  -Zi^
  -EHsc^
  -nologo^
  -I "../include"^
  -I "../ext/gl3w/include"^
  -I "../ext/imgui/include"^
  -I "../ext/glm"^
  ../src/main.cpp^
  shader.obj^
  gl3w.obj^
  imgui.obj^
  draw.obj^
  glfw_gl3.obj^
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
