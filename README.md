# VM Detector
A small set of functions for RE detection on x86_64 Linux
***
### Functions:
* `vmd_vmdetect()`
detects virtual machines via `cpuid` and `rdtscp`
* `vmd_hvdetect()`
detects hypervisors via `cpuid`
* `vmd_dbgpresent()`
detects debuggers via `ptrace`
* `vmd_inchroot()`
detects chroots via `inode`
* `vmd_hardwaresus()`
detects virtual machines via uncommonly low-end hardware
* `vmd_incontainer()`
detects containerization via `getpid`
### To use:
Run `make` or simply include `vmd.h` in your project. Define `VMD_IMPLEMENTATION` somewhere in your program if you aren't using vmd from a library.

#### Expected output from example:

```
strace: Debugger detected
qemu: VM detected
Hypervisor detected
docker: Chroot detected
Container detected
gdb: Debugger detected
valgrind: VM detected
vmd: Nothing detected
```
