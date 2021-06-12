#include <stdio.h>

#define VMD_IMPLEMENTATION
#include "vmd.h"
#include "syscall.h" //syscall by numbers for better inlining

int main(){
	const char  vm_string[] = "VM detected\n";
	const char  hv_string[] = "Hypervisor detected\n";
	const char dbg_string[] = "Debugger detected\n";
	const char chr_string[] = "Chroot detected\n";
	const char hws_string[] = "Hardware has suspicious configuration\n";
	const char k8s_string[] = "Container detected\n";
	const char  all_clear[] = "Nothing detected\n";
	int clear = 0;
	if(vmd_vmdetect())   {++clear; puts( vm_string);}
	if(vmd_hvdetect())   {++clear; puts( hv_string);}
	if(vmd_dbgpresent()) {++clear; puts(dbg_string);}
	if(vmd_inchroot())   {++clear; puts(chr_string);}
	if(vmd_hardwaresus()){++clear; puts(hws_string);}
	if(vmd_incontainer()){++clear; puts(k8s_string);}
	
	if(!clear){
		puts(all_clear);
	}
}
