#ifndef CPUID_h_gh387
#define CPUID_h_gh387

#include <stdint.h>

static uint32_t cpuid_eax, cpuid_ebx, cpuid_ecx, cpuid_edx;

#ifdef WIN32

#define CPUID CPUID_func()

void CPUID_func() {
	__asm {
		// store old eax and ecx values
		push eax
		push ecx
		mov eax, cpuid_eax
		mov ecx, cpuid_ecx
		cpuid
		mov cpuid_eax, eax
		mov cpuid_ebx, ebx
		mov cpuid_ecx, ecx
		mov cpuid_edx, edx
		pop ecx
		pop eax
	}
}

#else

/* this was taken from the linux kernel */
#define CPUID                              \
    __asm__ volatile ("cpuid"                             \
        : "=a" (cpuid_eax),     \
          "=b" (cpuid_ebx),     \
          "=c" (cpuid_ecx),     \
          "=d" (cpuid_edx)      \
        : "0" (cpuid_eax), "2" (cpuid_ecx))


#endif

#endif
