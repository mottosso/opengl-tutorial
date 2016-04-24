#include <stdio.h>
#include <wchar.h>

extern "C" {

__declspec(dllexport)
void print_hello(const wchar_t* u) {
    wprintf(L"hello: %s\n", u);
}

} // extern "C"