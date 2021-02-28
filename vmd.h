// (c)m1lkweed 2021 MIT License

#ifndef _VMD_H_
#define _VMD_H_

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <sys/ptrace.h>
#include <sys/sysinfo.h>

#include "syscall.h"

/*cpuid takes 200 times longer in VMs while x87 insns are roughly the same*/
bool vmd_vmdetect(void){
	unsigned long long t0, t1;
	unsigned junk = 0;
	double fjunk = 0;
	t0 = __builtin_ia32_rdtscp(&junk);
	__asm volatile("cpuid"
	              :"=a" (junk),
	               "=b" (junk),
	               "=c" (junk),
	               "=d" (junk)
	);
	t1 = __builtin_ia32_rdtscp(&junk);
	signed long long cpuid_time = t1 - t0;
	t0 = __builtin_ia32_rdtscp(&junk);
	__asm volatile("fbstp %0"
	              :"=m" (junk)
	              :"t" (fjunk)
	              :"st"
	);
	t1 = __builtin_ia32_rdtscp(&junk);
	signed long long fbstp_time = t1 - t0;
	if((fbstp_time) >= cpuid_time)
		return true;
	return false;
}

/*time the length of a SIGTRAP*/
bool vmd_dbgpresent(void){
	unsigned long long t0, t1;
	unsigned junk;
	struct sigaction old, new = {.sa_handler = SIG_IGN};
	syscall(SYS_rt_sigaction, SIGTRAP, (size_t)&new, (size_t)&old, 8);
	t0 = __builtin_ia32_rdtscp(&junk);
		syscall(SYS_kill, syscall(SYS_getpid), SIGTRAP);
	t1 = __builtin_ia32_rdtscp(&junk);
	syscall(SYS_rt_sigaction, SIGTRAP, (size_t)&old, (size_t)NULL, 8);
	if(((t1 - t0) < 190000) && ((t1 - t0) > 1000))
		return false;
	return true;
}

// / should have an inode of 2 if not chrooted, not foolproof
// but doesn't require root or checking for default strings
bool vmd_inchroot(void){
	struct stat a;
	char *dir = "/";
	syscall(SYS_stat, (size_t)dir, (size_t)&a);
	if(a.st_ino == 2)
		return false;
	return true;
}

/*tries to detect VMs by very low hardware specs*/
bool vmd_hardwaresus(void){
	struct sysinfo a;
	uint64_t size;
	if(get_nprocs_conf() < 2)
		return true;
	sysinfo(&a);
	if((a.totalram/(1024UL * 1024UL * 1024UL)) < 1) //< 1 GiB of RAM
		return true;
	if(a.uptime < (5 * 60)) //< 5 minutes of uptime
		return true;
	FILE *d = fopen("/sys/class/block/sda/size", "rx");
	fscanf(d,"%"SCNu64, &size);
	fclose(d);
	size *= 512;
	size /= 1024UL * 1024UL * 1024UL;
	if(size <= 60) //< 60 GiB total on main drive
		return true;
	return false;
}

/*tries to detect containerization by pid*/
bool vmd_incontainer(void){
	ssize_t pid = syscall(SYS_getpid);
	if(pid < 250)
		return true;
	return false;
}

#endif //_VMD_H_
