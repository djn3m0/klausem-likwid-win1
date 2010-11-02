
#include <windows.h>

#include <stdio.h>
#include <string.h>

BOOL loadDriver(const char *filename, const char *drivername)
{
    SC_HANDLE	hSCManager;
    SC_HANDLE	hService;
    SERVICE_STATUS	serviceStatus;
    BOOL	ret;

	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!hSCManager)
		return FALSE;

	hService = OpenService( hSCManager,
							drivername,
							SERVICE_ALL_ACCESS);
	if(hService){
		ret = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);
		ret = DeleteService(hService);
		CloseServiceHandle(hService);
	}

	hService = CreateService(hSCManager,
		drivername,
		drivername,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		filename,
		NULL,NULL,NULL,NULL,NULL);

	ret = FALSE;
    if(hService) {

		ret = StartService(hService, 0, NULL);

		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSCManager);

	return ret;
}

BOOL unLoadDriver(const char *drivername)
{
	SC_HANDLE	hSCManager;
	SC_HANDLE	hService;
	SERVICE_STATUS  serviceStatus;
	BOOL	ret;

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!hSCManager)
		return FALSE;

    hService = OpenService(hSCManager, drivername, SERVICE_ALL_ACCESS);
	ret = FALSE;
	if(hService) {
	    ret = ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);

	    if(ret == TRUE)
	        ret = DeleteService(hService);

	    CloseServiceHandle(hService);
	}
    CloseServiceHandle(hSCManager);

    return ret;
}

HANDLE openDriver(const char *driverName)
{
	char fullDriverName[1000];
	if (strlen(driverName) > 500) {
		fprintf(stderr, "driver name too long");
	}
	sprintf(fullDriverName, "\\\\.\\%s", driverName);
	return CreateFile(
		fullDriverName,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}
