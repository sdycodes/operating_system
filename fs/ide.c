/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

//extern int read_sector(int diskno, int offset);
//extern int write_sector(int diskno, int offset);

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurred during read the IDE disk, panic. 
// 	
// Hint: use syscalls to access device registers and buffers
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{  
	// 0x200: the size of a sector: 512 bytes.
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
    int r;
    int disk_offset;
    const char rw = 0;
	while (offset_begin  + offset < offset_end) {
        // Your code here
        disk_offset = offset_begin+offset;
        // set the disk register
        syscall_write_dev(&diskno, 0x13000010, 4);
        syscall_write_dev(&disk_offset, 0x13000000, 4);
        syscall_write_dev(&rw, 0x13000020, 1);
        // read the sector into memory
        syscall_read_dev(dst + offset, 0x13004000, 0x200);
        // read the return value of the disk 
        syscall_read_dev(&r, 0x13000030, 4);
        if(r<0) user_panic("read_sector error!!!\n");
        
        offset += 0x200;
        // error occurred, then panic.
	}

}


// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurred during read the IDE disk, panic.
//	
// Hint: use syscalls to access device registers and buffers
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{   
    // Your code here
	int offset_begin = secno * 0x200;
	int offset_end = offset_begin + nsecs * 0x200;
	int offset = 0;
    int r = 0;
    const char w = 1;
    int disk_offset;
	writef("diskno: %d\n", diskno);
	while (offset_begin + offset < offset_end) {
        
	    // copy data from source array to disk buffer.
        disk_offset = offset_begin + offset;
        // write data from memroy to buf 
        syscall_write_dev(src + offset, 0x13004000, 0x200);
        // set the disk register
        syscall_write_dev(&diskno, 0x13000010, 4);
        syscall_write_dev(&disk_offset, 0x13000000, 4);
        syscall_write_dev(&w, 0x13000020, 1);
        // check if success
        syscall_read_dev(&r, 0x13000030, 4);
        if(r<0) user_panic("write_sector errror!!\n");
        
        offset += 0x200;
        // if error occur, then panic.
  	 }
}

