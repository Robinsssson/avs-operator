#include "lib/avaspecx64.h"
#include <stdio.h>
#include <windows.h>

typedef int (*__AVS_Init)(short);
typedef int (*__AVS_GetDLLVersion)(char*);
int main(int argc, char** argv)
{
    HMODULE hDll = LoadLibrary("avaspecx64.dll"); // 替换为你的 DLL 文件名
    if (hDll == NULL) {
        printf("Failed to load DLL\n");
        return -1;
    }

    __AVS_Init AVS_Init = (__AVS_Init)GetProcAddress(hDll, "AVS_Init"); // 替换为 API 的函数名
    __AVS_GetDLLVersion AVS_GetDLLVersion = (__AVS_GetDLLVersion)GetProcAddress(hDll, "AVS_GetDLLVersion"); // 替换为 API 的函数名
    if (AVS_Init == NULL) {
        printf("Could not locate the function\n");
        FreeLibrary(hDll);
        return -1;
    }
    char str[1000];
    int result = AVS_Init(0); // 使用导入的函数
    AVS_GetDLLVersion(str);
    printf("Result: %s\n", str);

    FreeLibrary(hDll); // 释放 DLL
    return 0;
}
