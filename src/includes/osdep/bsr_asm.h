#ifdef WIN32

#include <stdint.h>

#define ASM_BSR(fieldWidth, number) ASM_BSR_func(&fieldWidth, number)

uint32_t ASM_BSR_func(uint32_t *fieldWidth, uint32_t number) {
	uint32_t res;
	__asm {
		mov eax, number
		bsr ecx, eax
		mov res, ecx
	}
	*fieldWidth = res;
}


#else

#define ASM_BSR(fieldWidth, number) __asm__ volatile ( "bsr %%eax, %%ecx\n\t" \
            : "=c" (fieldWidth) \
            : "a"(number))


#endif
