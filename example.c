#include "vmd.h"
#include "syscall.h" //syscall by numbers for better inlining

int main(){
	const char  vm_string[] = "VM detected\n";
	const char dbg_string[] = "Debugger detected\n";
	const char chr_string[] = "Chroot detected\n";
	const char hws_string[] = "Hardware has suspicious configuration\n";
	const char k8s_string[] = "Container detected\n";
	const char  all_clear[] = "Nothing detected\n";
	int clear = 0;
	if(vmd_vmdetect())   {++clear; write(1,  vm_string, sizeof( vm_string));}
	if(vmd_dbgpresent()) {++clear; write(1, dbg_string, sizeof(dbg_string));}
	if(vmd_inchroot())   {++clear; write(1, chr_string, sizeof(chr_string));}
	if(vmd_hardwaresus()){++clear; write(1, hws_string, sizeof(hws_string));}
	if(vmd_incontainer()){++clear; write(1, k8s_string, sizeof(k8s_string));}
	
	if(!clear){
		write(1, all_clear, sizeof(all_clear));
	}
}

