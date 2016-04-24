@echo off

:: Prepare build directory
mkdir build
pushd build

:: Compile
cl^
  -nologo^
  -D_USRDLL^
  -D_WINDLL^
  ../src/mydll.cpp^
  -MT^
  /link^
    /DLL^
    /OUT:mydll.dll

:: Install
copy /Y mydll.lib ..\lib\
copy /Y mydll.dll ..\lib\

popd
