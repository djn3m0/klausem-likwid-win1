#ifdef WIN32

// code taken from http://www.dalun.com/blogs/05.01.2007.htm

#include <windows.h>
#include <malloc.h>
#include <stdio.h>
#include <tchar.h>
#include <osdep/numOfProcessors.h>

typedef BOOL (WINAPI *LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
    PDWORD);

uint32_t numOfProcessors()
{
    BOOL done;
    BOOL rc;
    DWORD returnLength;
    DWORD procCoreCount;
    DWORD byteOffset;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer, ptr;
    LPFN_GLPI Glpi;

    Glpi = (LPFN_GLPI) GetProcAddress(
                            GetModuleHandle(TEXT("kernel32")),
                            "GetLogicalProcessorInformation");
    if (NULL == Glpi)
    {
        _tprintf(
            TEXT("GetLogicalProcessorInformation is not supported.\n"));
        exit(1);
    }

    done = FALSE;
    buffer = NULL;
    returnLength = 0;

    while (!done)
    {
        rc = Glpi(buffer, &returnLength);

        if (FALSE == rc)
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer)
                    free(buffer);

                buffer=(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
                        returnLength);

                if (NULL == buffer)
                {
                    _tprintf(TEXT("Allocation failure\n"));
                    return (2);
                }
            }
            else
            {
                _tprintf(TEXT("Error %d\n"), GetLastError());
                return (3);
            }
        }
        else done = TRUE;
    }

    procCoreCount = 0;
    byteOffset = 0;

    ptr=buffer;
    while (byteOffset < returnLength)
    {
        switch (ptr->Relationship)
        {
            case RelationProcessorCore:
                procCoreCount++;
                break;

            default:
                break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

    free (buffer);

	return procCoreCount;
}

#else

#include <osdep/numOfProcessors.h>
#include <stdio.h> //popen
#include <stdlib.h> //exit

uint32_t numOfProcessors() {
	FILE* pipe;

	uint32_t numProcs = -1;

	/* First determine the number of processors accessible */
	pipe = popen("cat /proc/cpuinfo | grep ^processor | wc -l", "r");
	if (fscanf(pipe, "%u\n", &numProcs) != 1)
	{
		fprintf(stderr, "Failed to fscanf cpuinfo!\n");
		exit(EXIT_FAILURE);
	}

	if (pclose(pipe) == -1) {
		fprintf(stderr, "Failed to close pipe for cpuinfo!\n");
		exit(EXIT_FAILURE);
	}

	return numProcs;
}

#endif
