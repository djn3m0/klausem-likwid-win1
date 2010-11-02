#ifndef LIKWID_MSR_TYPES_H
#define LIKWID_MSR_TYPES_H

#pragma pack(1)
typedef struct {
	unsigned __int32 processorId;
	__int32 reg;
	__int64 data;
} MsrWriteData_in;
# pragma pack ()

#pragma pack(1)
typedef struct {
	unsigned __int32 processorId;
	__int32 reg;
} MsrReadData_in;
# pragma pack ()

#endif
