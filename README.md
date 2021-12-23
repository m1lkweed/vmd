# VM Detector [![build](https://github.com/m1lkweed/vmd/actions/workflows/build.yml/badge.svg)](https://github.com/m1lkweed/vmd/actions/workflows/build.yml)
A small set of functions for RE detection on x86_64 Linux
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
`make test`
```
strace: Debugger detected
ltrace: Debugger detected
qemu: VM detected
Hypervisor detected
docker: Chroot detected
Container detected
gdb: Debugger detected
valgrind: VM detected
firejail: Debugger detected
Container detected
standalone: Nothing detected
```
#### More results:
|command|result|
|:---|:---|
`lldb ./vmd`|`Debugger detected`
`ddd ./vmd`|`Debugger detected`
`./x64determiniser ./vmd`|`VM detected`	`Debugger detected`
`edb --run ./vmd`|`Debugger detected`
`nemiver ./vmd`|`Debugger detected`
`radare2 -d ./vmd`|`Debugger detected`
`rr record -n ./vmd`|`VM detected`	`Hardware has suspicious configuration`
