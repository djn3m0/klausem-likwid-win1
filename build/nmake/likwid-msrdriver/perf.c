#include <ntddk.h>

#include "types.h"
#include "ioctlcodes.h"

#define NT_DEVICE_NAME      L"\\Device\\likwidmsrdriver"     // textual name of the driver
#define DOS_DEVICE_NAME     L"\\DosDevices\\likwidmsrdriver" // what is passed to CreateFile (driver name)

#define ENABLE_RDPMC_IN_RING3 0

//**********************************************************************************************
// Forward declarations of functions......
//**********************************************************************************************

//**********************************************************************************************
NTSTATUS PerfOpen
(
	IN	PDEVICE_OBJECT		pDeviceObject,
    IN	PIRP				pIrp
);

//**********************************************************************************************
NTSTATUS PerfClose
(
	IN	PDEVICE_OBJECT		pDeviceObject,
	IN	PIRP				pIrp
);

//**********************************************************************************************
NTSTATUS PerfWRMSR
(
	IN	PDEVICE_OBJECT		pDeviceObject,
	IN	PIRP				pIrp
);

//**********************************************************************************************
NTSTATUS PerfRDMSR
(
	IN	PDEVICE_OBJECT		pDeviceObject,
	IN	PIRP				pIrp
);


//**********************************************************************************************
__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
DRIVER_DISPATCH CentralDispatch;
NTSTATUS CentralDispatch
(
	IN	PDEVICE_OBJECT		pDeviceObject,
	IN	PIRP				pIrp             
);

//**********************************************************************************************
DRIVER_UNLOAD PerfUnload;
void PerfUnload
(
	IN	PDRIVER_OBJECT		pDriverObject
);

//**********************************************************************************************
DRIVER_INITIALIZE DriverEntry;



//**********************************************************************************************
// Main driver begins here.....
//**********************************************************************************************


//**********************************************************************************************
// This is the main driver entry point, similar to main in a standard C prog
// This is called when the driver is started, wither through net start [driver] or by setting it
// auto start in the control panel.
//
NTSTATUS DriverEntry
(
	IN PDRIVER_OBJECT DriverObject,
	IN PUNICODE_STRING RegistryPath
)
{
    PDEVICE_OBJECT		deviceObject = NULL;
    NTSTATUS			status;
    UNICODE_STRING		uniNtNameString;
    UNICODE_STRING		uniWin32NameString;

    KdPrint(("Perf: Entered the Load/Unload driver!\n"));

    //
    // Create counted string version of our device name.
    //
    RtlInitUnicodeString( &uniNtNameString, NT_DEVICE_NAME );


    //
    // Create the device object
    //
    status = IoCreateDevice
			(
				DriverObject,
				0,						// We don't use a device extension
				&uniNtNameString,
				DEVICE_FILE_TYPE,		// defined in header
				0,						// No standard device characteristics
				FALSE,					// This isn't an exclusive device
				&deviceObject
			);

    if ( NT_SUCCESS(status) )
    {

        //
        // All major functions to to a central displach function
        //
        DriverObject->MajorFunction[IRP_MJ_CREATE]			= CentralDispatch;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]			= CentralDispatch;
	    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = CentralDispatch;

		// The unload is the exception
        DriverObject->DriverUnload							= PerfUnload;

        KdPrint(("Perf: just about ready!\n"));

        //
        // Create counted string version of our Win32 device name.
        //    
        RtlInitUnicodeString( &uniWin32NameString, DOS_DEVICE_NAME );
    
        //
        // Create a link from our device name to a name in the Win32 namespace.
        //        
        status = IoCreateSymbolicLink( &uniWin32NameString, &uniNtNameString );

        if (!NT_SUCCESS(status))
        {
            KdPrint(("Perf: Couldn't create the symbolic link\n"));
            IoDeleteDevice( DriverObject->DeviceObject );
        }

		KdPrint(("Perf: Init complete\n"));
    }
    else
    {
        KdPrint(("PerfI: Couldn't create the device\n"));
    }
    return status;
}



//**********************************************************************************************
// When the driver is opened from a win32 prog this entry point is called.
//
NTSTATUS PerfOpen
(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PIRP pIrp
)
{
    KdPrint( ("Perf: Opened!!\n") );

	if (ENABLE_RDPMC_IN_RING3) {
		_asm
		{
			push	eax
			_emit	0x0f
			_emit	0x20
			_emit	0xe0			// mov eax,cr4

			// clear bit 2 to enable RDTSC at ring 3. [it should already be clear]
			and		eax, NOT (1<<2)

			// set bit 8 in cr4 to enable RDPMC at ring 3
			or		eax, (1<<8)

			_emit	0x0f
			_emit	0x22
			_emit	0xe0			// mov cr4,eax
			pop		eax
		}
	}

    return STATUS_SUCCESS;
}



//**********************************************************************************************
// When close handle is called from a win32 prog with the handle of the driver this entry point
// is called.
//
NTSTATUS PerfClose
(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
)
{
	//
	// we must put things back how we found them
	//
	if (ENABLE_RDPMC_IN_RING3) {
		_asm
		{
			push	eax
			_emit	0x0f
			_emit	0x20
			_emit	0xe0			// mov eax,cr4

			and		eax, NOT (1<<8)	// clear bit 8 to disable RDPMC from ring 3

			_emit	0x0f
			_emit	0x22
			_emit	0xe0			// mov cr4,eax
			pop		eax
		}
	}

	KdPrint( ("Perf: Closed!!\n") );

	return STATUS_SUCCESS;
}



//**********************************************************************************************
// This is called from the IOCTL system.
// This function will write to a model specific register
//
NTSTATUS PerfWRMSR
(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
)
{
	unsigned __int32    processorId;
	__int32             reg;
	__int32             data_low;
	__int32             data_high;
	PIO_STACK_LOCATION  pIrpStack;

	KdPrint(("Perf: Write model specific register\n"));

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	// we must have received 12 bytes of data.
	if ( pIrpStack->Parameters.DeviceIoControl.InputBufferLength != sizeof(MsrWriteData_in) )
	{
		pIrp->IoStatus.Information = 0;
		return STATUS_INVALID_PARAMETER;
	}
	// we do not expect to have received an output buffer
	if ( pIrpStack->Parameters.DeviceIoControl.OutputBufferLength != 0)
	{
		pIrp->IoStatus.Information = 0;
		return STATUS_INVALID_PARAMETER;
	}

	// NT copies inbuf here before entry and copies this to outbuf after
	// return, for METHOD_BUFFERED IOCTL's.
	{
		MsrWriteData_in* msrWriteData_in = (MsrWriteData_in*)pIrp->AssociatedIrp.SystemBuffer;

		// get the info out of the buffer
		processorId = msrWriteData_in->processorId;
		reg = msrWriteData_in->reg;
		data_low = ((__int32*)&msrWriteData_in->data)[0];
		data_high = ((__int32*)&msrWriteData_in->data)[1];
	}

	if (processorId >= KeQueryMaximumProcessorCount()) {
		pIrp->IoStatus.Information = 0;
		return STATUS_INVALID_PARAMETER;
	}
	{
		KAFFINITY oldAffinity = KeSetSystemAffinityThreadEx(1 << processorId);

		_asm
		{
			push	eax
			push	ecx
			push	edx

			mov		ecx, reg
			mov		eax, data_low
			mov		edx, data_high

			_emit	0x0f
			_emit	0x30            //WRMSR

			pop		edx
			pop		ecx
			pop		eax
		}

		KeRevertToUserAffinityThreadEx(oldAffinity);

		// writes return 0 bytes so a NULL dest buffer can be passed.
		pIrp->IoStatus.Information = 0;
	}

	return STATUS_SUCCESS;
}

/*NTSTATUS PerfWRMSR
(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
)
{
	ULONG				ul_reg;
	ULONG				ul_low;
	ULONG				ul_high;
	PIO_STACK_LOCATION	pIrpStack;
	PULONG				pul_buffer;

	KdPrint(("Perf: Write model specific register\n"));

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	// we must have sent at least 12 bytes of data.
	if ( pIrpStack->Parameters.DeviceIoControl.InputBufferLength < 12)
	{
		// not enough info has been sent so we had better not do anything otherwise a blue screen
		// core dump will result.
		return STATUS_INVALID_PARAMETER;
	}

	// NT copies inbuf here before entry and copies this to outbuf after
	// return, for METHOD_BUFFERED IOCTL's.
	pul_buffer = (PULONG)pIrp->AssociatedIrp.SystemBuffer;

	ul_reg = pul_buffer[0];
	ul_low = pul_buffer[1];		// get the info out of the buffer
	ul_high = pul_buffer[2];

	_asm
	{
		push	eax
		push	ecx
		push	edx

		mov		ecx, ul_reg
		mov		eax, ul_low
		mov		edx, ul_high

		_emit	0x0f
		_emit	0x30			//WRMSR
		
		pop		edx
		pop		ecx
		pop		eax
	}

	// writes return 0 bytes so a NULL dest buffer can be passed.
	pIrp->IoStatus.Information = 0;

	return STATUS_SUCCESS;
}*/



//**********************************************************************************************
// This is called from the IOCTL system.
// This function will read an model specific register.
//
NTSTATUS PerfRDMSR
(
	IN PDEVICE_OBJECT pDeviceObject,
	IN PIRP pIrp
)
{
	PIO_STACK_LOCATION  pIrpStack;
	PULONG              pul_buffer;
	unsigned __int32    processorId;
	__int32             reg;

	KdPrint(("Perf: Read model specific register\n"));

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	// check size of input buffer
	if ( pIrpStack->Parameters.DeviceIoControl.InputBufferLength != sizeof(MsrReadData_in))
	{
		// TODO: pIrp->IoStatus.Information = 0; ?? are buffers copied if parameters are invalid ?
		return STATUS_INVALID_PARAMETER;
	}

	// check size of output buffer
	if ( pIrpStack->Parameters.DeviceIoControl.OutputBufferLength != sizeof(__int64))
	{
		// TODO: pIrp->IoStatus.Information = 0; ??
		return STATUS_INVALID_PARAMETER;
	}

	// NT copies inbuf here before entry and copies this to outbuf after
	// return, for METHOD_BUFFERED IOCTL's.
	pul_buffer = (PULONG)pIrp->AssociatedIrp.SystemBuffer;

	// Take data out of buffer
	{
		MsrReadData_in* msrReadData_in = (MsrReadData_in*)pul_buffer;
		processorId = msrReadData_in->processorId;
		reg = msrReadData_in->reg;
	}

	// Check whether the processor ID is valid
	if (processorId >= KeQueryMaximumProcessorCount()) {
		return STATUS_INVALID_PARAMETER;
	}
	{
		KAFFINITY oldAffinity = KeSetSystemAffinityThreadEx(1 << processorId);

		__int32 data_low, data_high;

		_asm
		{
			push	eax
			push	ecx
			push	edx

			mov		ecx, reg

			_emit	0x0f
			_emit	0x32			//RDMSR

			mov		data_low, eax
			mov		data_high, edx

			pop		edx
			pop		ecx
			pop		eax
		}

		KeRevertToUserAffinityThreadEx(oldAffinity);

		pul_buffer[0] = data_low;   // put the result back into the buffer
		pul_buffer[1] = data_high;
	}

	// reads return an __int64
	pIrp->IoStatus.Information = sizeof(__int64);

	return STATUS_SUCCESS;
}




//**********************************************************************************************
//
void PerfUnload
(
	IN PDRIVER_OBJECT DriverObject
)
{
    UNICODE_STRING uniWin32NameString;

    KdPrint( ("Perf: Unloading!!\n") );
    
    //
    // Create counted string version of our Win32 device name.
    //
    RtlInitUnicodeString( &uniWin32NameString, DOS_DEVICE_NAME );

    //
    // Delete the link from our device name to a name in the Win32 namespace.
    //
    IoDeleteSymbolicLink( &uniWin32NameString );

    //
    // Finally delete our device object
    //
    IoDeleteDevice( DriverObject->DeviceObject );
}





//**********************************************************************************************
// This routine is the dispatch handler for the driver.  
// It is responsible for processing the IRPs. If routines called by this also process IRPs then
// you will get a blue screen crash due mutilple pending IRP results.
//
NTSTATUS CentralDispatch
(
	IN    PDEVICE_OBJECT pDO,
	IN    PIRP pIrp             
)
{
    PIO_STACK_LOCATION pIrpStack;
    NTSTATUS Status;

    pIrp->IoStatus.Information = 0;
    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

    //  Set default return status
    Status = STATUS_NOT_IMPLEMENTED;

	//
    // Dispatch based on major fcn code.
	//
    switch (pIrpStack->MajorFunction)
    {
		//
		// open
		//
        case IRP_MJ_CREATE:
            Status = PerfOpen(pDO, pIrp);
            break;

		//
		// Close
		//
        case IRP_MJ_CLOSE:
            Status = PerfClose(pDO, pIrp);
            break;

		//
		// IOCTL
		//
        case IRP_MJ_DEVICE_CONTROL:
            //  Dispatch on IOCTL
            switch (pIrpStack->Parameters.DeviceIoControl.IoControlCode)
            {
            case IOCTL_WRITE_MSR:
				Status = PerfWRMSR(pDO, pIrp);
                break;

            case IOCTL_READ_MSR:
				Status = PerfRDMSR(pDO, pIrp);
                break;

            }

            break;
    }

    // We're done. Record the status of the I/O action.
    pIrp->IoStatus.Status = Status;

    // Don't boost priority when returning since this took little time.
    IoCompleteRequest(pIrp, IO_NO_INCREMENT );

    return Status;
}
