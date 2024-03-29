// osdev.org - http://wiki.osdev.org/Setting_Up_Paging 
//             http://wiki.osdev.org/Paging
//             http://wiki.osdev.org/Page_Tables
// http://www.jamesmolloy.co.uk/tutorial_html/6.-Paging.html
//
#include "paging.h"
#include "common.h"
#include "kheap.h"
#include "drivers/serial_port.h" // pour afficher un msg
extern void enablePaging(page_directory_t*);

// NON PAE 4K Mode (32 bit) - level 0 page, level 1 PT, level 2 PD 
// The kernel's page directory
page_directory_t *page_directory=0;

// A bitset of frames - used or free.
u32int *frames;
u32int nframes;

// Defined in kheap.c
extern u32int placement_address;

page_t *get_page(u32int address, u8int make, page_directory_t *dir) {
  // Turn the address into an index.
  address /= 0x1000;
  // Find the page table containing this address.
  u32int tableIdx = address / 1024;
  // If this table is already assigned
  if (dir->tables[tableIdx]) {
    return &dir->tables[tableIdx]->pages[address % 1024];
  } else if (make) {
    u32int tmp;
    dir->tables[tableIdx] =
        (page_table_t *)kmalloc_page(sizeof(page_table_t), &tmp);
    memset((u8int *)dir->tables[tableIdx], 0, 0x1000);
    // PRESENT, RW, US.
    dir->tablesPhysical[tableIdx] = tmp | 0x7;
    return &dir->tables[tableIdx]->pages[address % 1024];
  } else {
    return 0;
  }
}

// Static function to set a bit in the frames bitset
static void set_frame(u32int frame_addr)
{
    u32int frame = frame_addr/0x1000;
    u32int frame_word = frame/(8*4);
    u32int bit_offset = frame%(8*4);
    frames[frame_word] |= (0x1 << bit_offset);
}

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	int idx=1;   // init une seule frame
        set_frame(idx*0x1000);
        page->present = 1;
        page->rw = (is_writeable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
}

//*
// paging initialisation 
//*
void init_paging() {
	u32int mem_end_page = 0x1000000; // taille memoire physique ex. 16MB
	nframes = mem_end_page / 0x1000; 
	frames = (u32int*)kmalloc(nframes/32);
	memset((u8int*)frames, 0, nframes/32);
 	
	page_directory = (page_directory_t*)kmalloc(sizeof(page_directory_t));

	u32int i = 0;
	while (i < placement_address)
	{
		// Kernel code is readable but not writeable from userspace.
		alloc_frame((page_t*)get_page(i, 1, page_directory), 0, 0);
		i += 0x1000;
	}
	 	
   	//register_interrupt_handler(14, handle_page_fault); // registrer interrupt before enabling paging
   	
	enablePaging(page_directory);

}

// page fault caught by interrup 
void page_fault() {
	char message[]= "Page Fault";
	serial_write(message,sizeof(message));
}
