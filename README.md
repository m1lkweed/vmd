# VM Detector
A small set of functions for RE detection on x86_64 Linux
***
### Functions:
* `vmd_vmdetect()`
detects virtual machines via `cpuid` and `rdtscp`
* `vmd_dbgpresent()`
detects debuggers via `SIGTRAP` timing
* `vmd_inchroot()`
detects chroots via `inode`
* `vmd_hardwaresus()`
detects virtual machines via uncommonly low-end hardware
* `vmd_incontainer()`
detects containerization via `getpid`
### To use:
Run `make` or simply include `vmd.h` in your project
