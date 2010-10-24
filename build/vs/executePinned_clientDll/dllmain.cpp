#include <windows.h>
#include <detours.h>
#include <stdio.h>
extern "C" {
#include <osdep/executePinned.h>
}
static __out_opt HANDLE (WINAPI * CreateThread_orig)(
	__in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt __deref __drv_aliasesMem LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __out_opt LPDWORD lpThreadId) = CreateThread;

__out_opt
HANDLE
WINAPI
CreateThread_detour(
    __in_opt  LPSECURITY_ATTRIBUTES lpThreadAttributes,
    __in      SIZE_T dwStackSize,
    __in      LPTHREAD_START_ROUTINE lpStartAddress,
    __in_opt __deref __drv_aliasesMem LPVOID lpParameter,
    __in      DWORD dwCreationFlags,
    __out_opt LPDWORD lpThreadId
    )
{
	//fprintf(stderr, "CreateThread_detour\n");
	HANDLE threadId = CreateThread_orig(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
	executePinned_pinNextThread(threadId);
	return threadId;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
	//fprintf(stderr, "executePinned_clientDll_dllMain\n");
    if (dwReason == DLL_PROCESS_ATTACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourAttach((PVOID*)&CreateThread_orig, CreateThread_detour);
        DetourTransactionCommit();
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourDetach((PVOID*)&CreateThread_orig, CreateThread_detour);
        DetourTransactionCommit();
    }
    return TRUE;
}
