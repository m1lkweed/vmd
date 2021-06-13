// (c)m1lkweed 2021 MIT License

#ifndef VMD_H_
#define VMD_H_

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
#include <sys/sysinfo.h>

#include "syscall.h"

/*cpuid takes 200 times longer in VMs while x87 insns are roughly the same*/
//may fail to detect fully virtualized machines, need a second test
bool vmd_vmdetect(void){
	unsigned long long t0, t1;
	unsigned junk = 0;
	double fjunk = 0;
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
	              :"=m" (junk)
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
bool vmd_dbgpresent(void){
	int failsafe = 0;
	if((syscall(SYS_ptrace, PTRACE_TRACEME, 0, 0, 0) == -1))
		failsafe = 2;
	if((syscall(SYS_ptrace, PTRACE_TRACEME, 0, 0, 0) == -1))
		failsafe *= 3;
	
	if(failsafe == 6 && syscall(SYS_ptrace, PTRACE_PEEKDATA, syscall(SYS_getpid), (uintptr_t)&failsafe, 0) == failsafe)
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
	return (a.st_ino != 2);
}

/*tries to detect VMs by very low hardware specs*/
bool vmd_hardwaresus(void){
	struct sysinfo a;
	uint64_t size;
	if(sysconf(_SC_NPROCESSORS_CONF) < 2)
		return true;
	sysinfo(&a);
	if((a.totalram/(1024UL * 1024UL * 1024UL)) < 1) // < 1 GiB of RAM
		return true;
	if(a.uptime < (5 * 60)) // < 5 minutes of uptime
		return true;
	FILE *d = fopen("/sys/class/block/sda/size", "rx");
	if(d == NULL)
		return true; //treat live CDs as suspect
	fscanf(d,"%"SCNu64, &size);
	fclose(d);
	size *= 512;
	size /= 1024UL * 1024UL * 1024UL;
	return (size <= 60); // < 60 GiB total on main drive
}

/*tries to detect containerization by pid*/
bool vmd_incontainer(void){
	ssize_t pid = syscall(SYS_getpid);
	return (pid < 250);
}

#endif //VMD_IMPLEMENTATION
#endif //VMD_H_
