// (c)m1lkweed 2021-2022 GPLv3+

#ifndef VMD_H_
#define VMD_H_

#include <stdlib.h>
#include <stdbool.h>

bool vmd_vmdetect(void);
bool vmd_hvdetect(void);
bool vmd_dbgpresent(void);
bool vmd_inchroot(void);
bool vmd_hardwaresus(void);
bool vmd_incontainer(void);

#ifdef VMD_IMPLEMENTATION

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <sys/ptrace.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <linux/limits.h>

#include "syscall.h"

/*cpuid takes 200 times longer in VMs while x87 insns are roughly the same*/
//may fail to detect fully virtualized machines, need a second test
bool vmd_vmdetect(void){
	unsigned long long t0, t1;
	unsigned junk = 0;
	unsigned char jarr[10] = {0};
	double fjunk = 0;
	volatile struct{
		int64_t base;
		int16_t offset;
	} a[2] = {0};
	asm("sgdt %0":"=m"(a[0])::);
	asm("sidt %0":"=m"(a[1])::);
	if(((a[0].offset == 0) || (a[0].base > 0)) || ((a[1].offset == 0) || (a[1].base > 0)))
		return true;
	/*small address filter so valgrind doesn't reach the SIGILL*/
	if(&t0 < (unsigned long long*)0x7ff000000000)
		return true;
	t0 = __builtin_ia32_rdtscp(&junk);
	__asm volatile("cpuid"
	              :"=a" (junk),
	               "=b" (junk),
	               "=c" (junk),
	               "=d" (junk)
	);
	t1 = __builtin_ia32_rdtscp(&junk);
	unsigned long long cpuid_time = t1 - t0;
	t0 = __builtin_ia32_rdtscp(&junk);
	__asm volatile("fbstp %0"
	              :"=m" (jarr)
	              :"t" (fjunk)
	              :"st"
	);
	t1 = __builtin_ia32_rdtscp(&junk);
	unsigned long long fbstp_time = t1 - t0;
	return (fbstp_time >= cpuid_time) || (cpuid_time > 2000);
}

/*detects hypervisors via cpuid*/
bool vmd_hvdetect(void){
	unsigned junk = 0, hypervisor = 0;
	__asm volatile("cpuid"
	              :"=a" (junk),
	               "=b" (junk),
	               "=c" (hypervisor),
	               "=d" (junk)
	              :"0"  (1),
	               "1"  (0),
	               "2"  (0)
	);
	return hypervisor & 0x80000000;
}

/*double self-trace to detect/prevent debuggers LD_PRELOAD*/
//TODO: In kernel 5.11 syscalls can now be captured, we should try reading from memory to confirm attachment.
bool vmd_dbgpresent(void){
	static unsigned char bss;
	unsigned char *probe = malloc(0x10);
	bool ret = (probe - &bss < 0x20000);
	extern char etext, __executable_start;
	if(((uintptr_t)(&etext) == (uintptr_t)0x55555555546d) || ((uintptr_t)(&__executable_start) == (uintptr_t)0x555555554000))
		return true;
	free(probe);
	if(ret)
		return true;
	int failsafe = 0;
	if((syscall(SYS_ptrace, PTRACE_TRACEME, 0, 0, 0) == -1))
		failsafe = 2;
	if((syscall(SYS_ptrace, PTRACE_TRACEME, 0, 0, 0) == -1))
		failsafe *= 3;
	if(failsafe == 6)
		return true;
	syscall(SYS_ptrace, PTRACE_DETACH, syscall(SYS_getpid), 0, 0);
	return false;
}

// / should have an inode of 2 if not chrooted, not foolproof
// but doesn't require root or checking for default strings
bool vmd_inchroot(void){
	struct stat a;
	char *dir = "/";
	syscall(SYS_stat, (size_t)dir, (size_t)&a);
	//TODO: only check for inode of 256 on btrfs
	return !((a.st_ino == 2) || (a.st_ino == 256));
}

/*tries to detect VMs by very low hardware specs*/
bool vmd_hardwaresus(void){
	struct sysinfo a;
	struct statvfs b;
	uint64_t size;
	if(sysconf(_SC_NPROCESSORS_CONF) < 2)
		return true;
	sysinfo(&a);
	if((a.totalram/(1024UL * 1024UL * 1024UL)) < 1) // < 1 GiB of RAM
		return true;
	if(a.uptime < (5 * 60)) // < 5 minutes of uptime
		return true;
	statvfs("/", &b);
	size = b.f_frsize * b.f_blocks;
	size /= 1024UL * 1024UL * 1024UL; // size to GiB
	return (size <= 14); // < 14 GiB total on main drive
}

#define VMD_STR2(x) #x
#define VMD_STR1(x) VMD_STR2(x)
#define VMD_STR(x)  VMD_STR1(x)
/*tries to detect containerization by pid*/
bool vmd_incontainer(void){
	int initpid;
	char junk[NAME_MAX + 1];
	FILE *d = fopen("/proc/1/sched", "rx");
	if(d == NULL)
		return true;
	fscanf(d,"%" VMD_STR(NAME_MAX) "s (%d", junk, &initpid);
	fclose(d);
	if(initpid != 1)
		return true;
	ssize_t pid = syscall(SYS_getpid);
	return (pid < 250);
}
#undef VMD_STR
#undef VMD_STR1
#undef VMD_STR2

#endif //VMD_IMPLEMENTATION
#endif //VMD_H_
