@echo off

mkdir build
pushd build

clang++^
    -o main^
    ../ext/imgui/src/imgui.cpp^
    ../ext/imgui/src/draw.cpp^
    ../ext/imgui/src/ref/glfw_gl2.cpp^
    ../src/main.cpp^
    -Wall^
    -g^
    -std=c++11^
    -I ../include^
    -I ../ext/glfw/include^
    -I ../ext/imgui/include^
    -L ../lib^
    -lglfw3^
    -lopengl32^
    -luser32^
    -lgdi32^
    -lcomdlg32^
    -ladvapi32^
    -lshell32^
    -lole32^
    -loleaut32

popd