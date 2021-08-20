    #include "drivers/frame_buffer.h"
    #include "drivers/serial_port.h"
    #include "segmentation/memory_segments.h"
    #include "interrupts.h"
    #include "keyboard.h"


    
    int main(){

           char ptr2[] = "begining to PrimitiveOS";
   
    
    fb_move_cursor(6*80);
    	
    	
    fb_write_str(ptr2,sizeof(ptr2));
	
	
    serial_write(ptr2,sizeof(ptr2));
    segments_install_gdt();
    interrupts_install_idt();
    
    
    }
    
