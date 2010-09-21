// executePinned_clientDll.cpp : Defines the exported functions for the DLL application.
//

// some exported function is required to make it possible for detours to work with the dll
__declspec(dllexport) void myDllFunc() {
}
