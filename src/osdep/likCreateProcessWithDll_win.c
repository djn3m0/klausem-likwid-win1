#include <stdio.h>
#include <windows.h>
#include <detours.h>
#include <osdep/windowsError_win.h>
#include <osdep/getFullLibraryFilename_win.h>

#define arrayLength(x)      (sizeof(x)/sizeof(x[0]))

//////////////////////////////////////////////////////////////////////////////
//
//  This code verifies that the named DLL has been configured correctly
//  to be imported into the target process.  DLLs must export a function with
//  ordinal #1 so that the import table touch-up magic works.
//
static BOOL CALLBACK ExportCallback(PVOID pContext,
                                    ULONG nOrdinal,
                                    PCHAR pszSymbol,
                                    PVOID pbTarget)
{
    (void)pContext;
    (void)pbTarget;
    (void)pszSymbol;

    if (nOrdinal == 1) {
        *((BOOL *)pContext) = TRUE;
    }
    return TRUE;
}

BOOL DoesDllExportOrdinal1(LPCTSTR pszDllPath)
{
    HMODULE hDll = LoadLibraryEx(pszDllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hDll == NULL) {
        printf("LoadLibraryEx(%s) failed with error %d.\n",
               pszDllPath,
               GetLastError());
		printLastWinError("Error Message");
        return FALSE;
    }

	{
		BOOL validFlag = FALSE;
		DetourEnumerateExports(hDll, &validFlag, ExportCallback);
		FreeLibrary(hDll);
		return validFlag;
	}
}

//////////////////////////////////////////////////////////////////////// main.
//
int likCreateProcessWithDll(
    const char *applicationName,
    const char *commandLine,
    const char *detouredDllPath,
    const char *injectedDllPath
    )
{
    /////////////////////////////////////////////////////////// Validate DLLs.
    //
    CHAR szDllPath[1024];
    CHAR szDetouredDllPath[1024];

	if (!getFullDllFilename(injectedDllPath, szDllPath, arrayLength(szDllPath))) {
		fprintf(stderr, "Error: Could not determine absolute path for dll %s.\n",
               injectedDllPath);
	}
    if (!DoesDllExportOrdinal1(injectedDllPath)) {
        printf("Error: %s does not export function with ordinal #1.\n",
               injectedDllPath);
        return 9003;
    }

    if (detouredDllPath != NULL) {
		if (!getFullDllFilename(detouredDllPath, szDetouredDllPath, arrayLength(szDetouredDllPath))) {
			fprintf(stderr, "Error: Could not determine absolute path for dll %s.\n",
				   injectedDllPath);
		}
        if (!DoesDllExportOrdinal1(detouredDllPath)) {
            printf("Error: %s does not export function with ordinal #1.\n",
                   detouredDllPath);
            return 9005;
        }
    }
    else {
        HMODULE hDetouredDll = DetourGetDetouredMarker();
        GetModuleFileName(hDetouredDll,
                          szDetouredDllPath, arrayLength(szDetouredDllPath));
#if 0
        if (!SearchPath(NULL, "detoured.dll", NULL,
                        arrayLength(szDetouredDllPath),
                        szDetouredDllPath,
                        &pszFilePart)) {
            printf("Couldn't find Detoured.DLL.\n");
            return 9006;
        }
#endif
    }

    //////////////////////////////////////////////////////////////////////////
	{
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		CHAR szCommand[2048];
		CHAR szFullExe[1024] = "\0";
		PCHAR pszFileExe = NULL;

		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		si.cb = sizeof(si);

		szCommand[0] = L'\0';

	#ifdef _CRT_INSECURE_DEPRECATE
		strcpy_s(szCommand, sizeof(szCommand), commandLine);
	#else
		strcpy(szCommand, commandLine);
	#endif
    
		/*printf("Starting: `%s'\n", szCommand);
		printf("  with `%s'\n\n", szDllPath);
		printf("  marked by `%s'\n\n", szDetouredDllPath);*/
		fflush(stdout);

		{
			DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;

			SetLastError(0);
			if (!DetourCreateProcessWithDll(applicationName[0] ? applicationName : NULL, szCommand,
											NULL, NULL, TRUE, dwFlags, NULL, NULL,
											&si, &pi, szDetouredDllPath, szDllPath, NULL)) {
				printLastWinError("DetourCreateProcessWithDll failed");
				ExitProcess(9007);
			}
		}

		ResumeThread(pi.hThread);

		WaitForSingleObject(pi.hProcess, INFINITE);

		{
			DWORD dwResult = 0;
			if (!GetExitCodeProcess(pi.hProcess, &dwResult)) {
				printf("GetExitCodeProcess failed: %d\n", GetLastError());
				return 9008;
			}

			return dwResult;
		}
	}
}
//
///////////////////////////////////////////////////////////////// End of File.
