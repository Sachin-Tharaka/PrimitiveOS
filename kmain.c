    #include "drivers/frame_buffer.h"
    #include "drivers/serial_port.h"
    


    
    int main(){

           char ptr2[] = "begining to PrimitiveOS";
   
    
    serial_write(0x3F8, ptr2, 23);
    fb_write(ptr2, 23);
    
    }
