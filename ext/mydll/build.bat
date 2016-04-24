@echo off

:: Prepare build directory
mkdir build
pushd build

:: Compile
cl^
  -Zi^
  -EHsc^
  -nologo^
  -I "../include"^
  %1^
  -Fe%2.exe^
  -link^
    /LIBPATH:"../lib"^
    /NODEFAULTLIB:LIBCMT

popd
