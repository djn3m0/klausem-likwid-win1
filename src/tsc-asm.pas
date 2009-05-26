FUNC readCycles
{
xor GPR1, GPR1
cpuid
rdtsc
mov [ARG1]  , eax
mov [ARG1+4], edx
xor GPR1, GPR1
cpuid
}



FUNC cyclesForCpuId ALLOCATE 3
{

xor GPR1, GPR1
cpuid
rdtsc
mov LOCAL1, GPR1
mov LOCAL2, GPR4
xor GPR1, GPR1
cpuid
rdtsc

xor GPR1, GPR1
cpuid
rdtsc
mov LOCAL1, GPR1
mov LOCAL2, GPR4
xor GPR1, GPR1
cpuid
rdtsc

xor GPR1, GPR1
cpuid
rdtsc
mov LOCAL1, GPR1
mov LOCAL2, GPR4
xor GPR1, GPR1
cpuid
rdtsc
sub GPR1, LOCAL1
mov LOCAL3, GPR1

xor GPR1, GPR1
cpuid
rdtsc
mov LOCAL1, GPR1
mov LOCAL2, GPR4
xor GPR1, GPR1
cpuid
rdtsc
sub GPR1, LOCAL1

cmp GPR1, LOCAL3
cmovl GPR1, LOCAL3

}

