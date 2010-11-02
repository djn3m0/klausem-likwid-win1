/*
 * ===========================================================================
 *
 *      Filename:  msr.c
 *
 *      Description:  Implementation of msr module.
 *                   Provides API to read and write values to the model
 *                   specific registers on x86 processors using the msr
 *                   sys interface of the Linux 2.6 kernel. This module 
 *                   is based on the msr-util tools.
 *
 *      Version:  <VERSION>
 *      Created:  <DATE>
 *
 *      Author:  Jan Treibig (jt), jan.treibig@gmail.com
 *      Company:  RRZE Erlangen
 *      Project:  likwid
 *      Copyright:  Copyright (c) 2010, Jan Treibig
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License, v2, as
 *      published by the Free Software Foundation
 *     
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *     
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * ===========================================================================
 */


/* #####   HEADER FILE INCLUDES   ######################################### */

#include <osdep/msr.h>

#include <stdio.h>
#include <stdint.h>

#include <osdep/windowsError_win.h>
#include <osdep/loaddriver_win.h>
#include <osdep/getFullLibraryFilename_win.h>
#include <likwid-msrdriver/ioctlcodes.h>
#include <likwid-msrdriver/types.h>

static HANDLE msr_driverHandle = NULL;

static int msr_logEnabled = 0;

void msr_writeToLog(const char *text) {
	if (msr_logEnabled) {
		static int calledFirstTime = 1;
		const char *openMode = calledFirstTime
			? "w"
			: "a";
		calledFirstTime = 0;

		{
			FILE * f = fopen ( "MsrLog.txt", openMode);
			fputs(text, f);
			//fputs("\n", f);
			fclose(f);
		}
	}
}

void msr_writeToLog_ULL(const char *format, unsigned int i) {
	char text[1000];
	sprintf(text, format, i);
	msr_writeToLog(text);
}

void msr_writeToLog_ULL_ULL(const char *format, unsigned int i1, unsigned int i2) {
	char text[1000];
	sprintf(text, format, i1, i2);
	msr_writeToLog(text);
}

void msr_writeToLog_ULL_ULL_ULL(const char *format, unsigned int i1, unsigned int i2, unsigned int i3) {
	char text[1000];
	sprintf(text, format, i1, i2, i3);
	msr_writeToLog(text);
}

void msr_writeToLog_ULL_ULL_ULL_ULL(const char *format, unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4) {
	char text[1000];
	sprintf(text, format, i1, i2, i3, i4);
	msr_writeToLog(text);
}

/* #####   FUNCTION DEFINITIONS  -  EXPORTED FUNCTIONS   ################## */
void
msr_init()
{
	char fullDriverFilename[1000];

	if (msr_driverHandle != NULL) return;

	getFullDllFilename("likwid-msrdriver.sys", fullDriverFilename, sizeof(fullDriverFilename)/sizeof(fullDriverFilename[0]));

	if (!loadDriver(fullDriverFilename, "likwidmsrdriver")) {
		printLastWinError("load driver likwid-msrdriver.sys");
		exit(1);
	}
	msr_driverHandle = openDriver("likwidmsrdriver");
	/*msr_driverHandle = OpenDriver(
	  L"F:\\Work\\Studium\\Aktivitaeten\\HPC_Hiwi\\win\\collection\\likwid\\build\\vs\\likwid-perfCtr\\likwid-msrdriver.sys",
	  NULL,
	  0);*/

	if (msr_driverHandle == INVALID_HANDLE_VALUE) {
		printLastWinError("Could not load MSR driver");
		exit(1);
	}
}

uint64_t
msr_read(int cpu, uint32_t reg)
{
	MsrReadData_in inData;
	__int64 outData;
	DWORD bytesReturned;
	BOOL success;

	msr_init();

	msr_writeToLog_ULL_ULL("read (cpu, reg) = (data1, data0) (%5x %5x) ", cpu, reg);

	inData.processorId = cpu;
	inData.reg = reg;

	success = DeviceIoControl
	(
		msr_driverHandle,           // Handle to device
		(DWORD) IOCTL_READ_MSR,     // IO Control code for Read
		&inData,                    // Input Buffer to driver.
		sizeof(inData),             // Length of input buffer.
		&outData,					    // Output Buffer from driver.
		sizeof(__int64),           // Length of output buffer in bytes.
		&bytesReturned,             // Bytes placed in output buffer.
		NULL                        // NULL means wait till op. completes
	);

	if (bytesReturned != sizeof(__int64)) {
		fprintf(stderr, "READ_MSR call sent to MSR driver returned %u bytes instead of %u\n", bytesReturned, sizeof(uint64_t));
		printLastWinError("Last Windows error");
		exit(1);
	}
	if (!success) {
		printLastWinError("READ_MSR");
		exit(1);
	}

	msr_writeToLog_ULL_ULL(" = (%5x %5x)\n", HIWORD(outData), LOWORD(outData));

	return outData;
}

void 
msr_write(int cpu, uint32_t reg, uint64_t data)
{
    BOOL  success;
	MsrWriteData_in msrWriteData_in;
	DWORD bytesReturned = 12345;

	msr_init();

	msrWriteData_in.processorId = cpu;
	msrWriteData_in.reg = reg;
	msrWriteData_in.data = data;

	msr_writeToLog_ULL_ULL_ULL_ULL("write (cpu, reg, data1, data0)   (%5x %5x %5x %5x)\n", cpu, reg, HIWORD(data), LOWORD(data) );
	success = DeviceIoControl
	(
		msr_driverHandle,           // Handle to device
		(DWORD) IOCTL_WRITE_MSR,    // IO Control code for Read
		&msrWriteData_in,           // Input Buffer to driver.
		sizeof(MsrWriteData_in),    // Length of Input buffer
		NULL,                       // Buffer from driver, None for WRMSR
		0,                          // Length of output buffer in bytes.
		&bytesReturned,             // Bytes placed in DataBuffer.
		NULL                        // NULL means wait till op. completes.
	);

	if (bytesReturned!=0) {
		fprintf(stderr, "WRITE_MSR call sent to MSR driver returned %u bytes instead of 0 bytes\n", bytesReturned);
		printLastWinError("Last Windows error");
		exit(1);
	}
	if (!success) {
		printLastWinError("WRITE_MSR");
		exit(1);
	}
}
