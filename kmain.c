    #include "drivers/io.h"
    #include "drivers/frame_buffer.h"
    #include "drivers/serial_port.h"
    #include "segmentation/memory_segments.h"
    #include "interrupts.h"
    #include "keyboard.h"
    #include "multiboot.h"


    
    int run_user_mode(unsigned int ebx)
{
	multiboot_info_t* mbinfo = (multiboot_info_t*) ebx;
  	multiboot_module_t* modules = (multiboot_module_t*) mbinfo->mods_addr; 
  	unsigned int address_of_module = modules->mod_start;
  	
  	if((mbinfo->mods_count) == 1){
  		char message[] = "Only ONE module loaded successfully!";
  		serial_write(message,sizeof(message));
  		
  		typedef void (*call_module_t)(void);
        	/* ... */
        	call_module_t start_program = (call_module_t) address_of_module;
        	start_program();
        	/* we'll never get here, unless the module code returns */

  	}else{
  		char message[] = "Error!: More than ONE module loaded";
  		serial_write(message,sizeof(message));
  	}
  	 
  	return 0;
}

int kmain(unsigned int ebx)
{
	char str[] = "Welcome to PrimitiveOS";
	
    	/* move cursor */
    	fb_move_cursor(6*80);
    	
    	/* frame builder writing */
	fb_write_str(str,sizeof(str));
	
	
	/* install segments */
	segments_install_gdt();
    
	/* install interrupt handler */
	interrupts_install_idt();
	
	/* call user mode */
	run_user_mode(ebx);
    	
	return 0;
}
