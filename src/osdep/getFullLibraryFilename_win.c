#include <osdep/getFullLibraryFilename_win.h>
#include <osdep/windowsError_win.h>
#include <stdio.h>

BOOL getFullDllFilename(LPCTSTR pszDllPath, PCHAR absDllFilename, int absDllFilenameLength) {
	HMODULE hDll = LoadLibraryEx(pszDllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hDll == NULL) {
        printf("LoadLibraryEx(%s) failed with error %d.\n",
               pszDllPath,
               GetLastError());
		printLastWinError("Error Message");
        return FALSE;
    }

	{
		GetModuleFileName((HINSTANCE)hDll, absDllFilename, _MAX_PATH);
		FreeLibrary(hDll);
		return TRUE;
	}
}
