#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <types.h>


uint64 readMSR(int cpu, uint32 reg)
{
    int  fd;
    uint64 data;
    char msr_file_name[64];

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_RDONLY);
    if (fd < 0) {
	if (errno == ENXIO) {
	    fprintf(stderr, "rdmsr: No CPU %d\n", cpu);
	    exit(2);
	} else if (errno == EIO) {
	    fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
		    cpu);
	    exit(3);
	} else {
	    perror("rdmsr: open");
	    exit(127);
	}
    }

    if (pread(fd, &data, sizeof data, reg) != sizeof data) {
	if (errno == EIO) {
	    fprintf(stderr, "rdmsr: CPU %d cannot read MSR %x\n",
		    cpu, reg);
	    exit(4);
	} else {
	    perror("rdmsr: pread");
	    exit(127);
	}
    }

    close(fd);
    return data;
}

void writeMSR(int cpu, uint32 reg, uint64 data)
{
    int  fd;
    char msr_file_name[64];

    sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
    fd = open(msr_file_name, O_WRONLY);
    if (fd < 0) {
	if (errno == ENXIO) {
	    fprintf(stderr, "wrmsr: No CPU %d\n", cpu);
	    exit(2);
	} else if (errno == EIO) {
	    fprintf(stderr, "wrmsr: CPU %d doesn't support MSRs\n",
		    cpu);
	    exit(3);
	} else {
	    perror("wrmsr: open");
	    exit(127);
	}
    }

    if (pwrite(fd, &data, sizeof data, reg) != sizeof data) {
	if (errno == EIO) {
	    fprintf(stderr,
		    "wrmsr: CPU %d cannot set MSR %X to %llX\n",
		    cpu, reg, data);
	    exit(4);
	} else {
	    perror("wrmsr: pwrite");
	    exit(127);
	}
    }

    close(fd);
}


