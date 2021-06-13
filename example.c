#include <stdio.h>

#define VMD_IMPLEMENTATION
#include "vmd.h"

int main(){
	const char  vm_string[] = "VM detected";
	const char  hv_string[] = "Hypervisor detected";
	const char dbg_string[] = "Debugger detected";
	const char chr_string[] = "Chroot detected";
	const char hws_string[] = "Hardware has suspicious configuration";
	const char k8s_string[] = "Container detected";
	const char  all_clear[] = "Nothing detected";
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
