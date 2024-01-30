#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net.h"
#include "mdadm.h"
#include "jbod.h"

uint32_t pack_bytes(uint32_t DiskID, uint32_t BlockID, uint32_t Command, uint32_t Reserved) {
	uint32_t retval = 0x0, tempa, tempb, tempc, tempd;
	tempa = DiskID;
	tempb = (BlockID) << 4;
	tempc = (Command) << 12; 
	tempd = (Reserved) << 20; 
	retval = tempa|tempb|tempc|tempd; 
	//printf("DiskID: 0x%08x\n", tempa);
	//printf("BlockID: 0x%08x\n", tempb);
	//printf("Command: 0x%08x\n", tempc);
	//printf("Reserved: 0x%08x\n", tempd);
	return retval;
}

int is_mounted = 0;
int mdadm_mount(void) {
	uint32_t op =pack_bytes(0,0,JBOD_MOUNT,0);
	jbod_client_operation(op,NULL);
	if (is_mounted == 0){
		is_mounted = 1;
		return 1;
	}

	return -1;
}

int mdadm_unmount(void) {
	uint32_t op =pack_bytes(0,0,JBOD_UNMOUNTED,0);
	jbod_client_operation(op,NULL);
	if(is_mounted == 1){
		is_mounted = 0;
		return 1;
	}
  
	return -1;
}
int has_permission = 0;
int mdadm_write_permission(void){
	uint32_t op =pack_bytes(0,0,JBOD_WRITE_PERMISSION,0);
	jbod_client_operation(op,NULL);
	if(has_permission == 0){
		has_permission = 1;
		return 1;
	}
	
	return 1;
}

int mdadm_revoke_write_permission(void){
	uint32_t op =pack_bytes(0,0,JBOD_REVOKE_WRITE_PERMISSION,0);
	jbod_client_operation(op,NULL);
	if(has_permission == 1){
		has_permission = 0;
		return 1;
	}

	return 0;
}


int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)  {
	if(start_addr + read_len > JBOD_NUM_DISKS * JBOD_DISK_SIZE){
		return -1;
	}
	if(read_len>1024){
		return -1;
	}
	if(is_mounted!=1){
		return -1;
	}
	if(read_buf == NULL && read_len != 0){
		return -1;
	}
	int bytes_read = 0;
	uint8_t temp_buff[JBOD_BLOCK_SIZE];
	int BlockID = 0; 
	int DiskID = 0; 
	int offset =0;  //start_addr%JBOD_BLOCK_SIZE;
	
	//int Command = 0; 
	 
	int rem=read_len;
	uint32_t bytes_to_be_read;
	while (bytes_read < read_len){


		DiskID = start_addr/JBOD_DISK_SIZE;
		BlockID = start_addr%JBOD_DISK_SIZE / JBOD_NUM_BLOCKS_PER_DISK;
		offset = start_addr%JBOD_BLOCK_SIZE;


		uint32_t op = pack_bytes(DiskID,0,JBOD_SEEK_TO_DISK,0);
		if (jbod_client_operation(op,NULL) != 0) return -1;


		op = pack_bytes(0,BlockID,JBOD_SEEK_TO_BLOCK,0);
		if(jbod_client_operation(op,NULL) != 0) return -1;

		op = pack_bytes(0,0,JBOD_READ_BLOCK,0);
		if(jbod_client_operation(op,temp_buff) != 0) return -1;


		if(bytes_read==0){
			if(rem+offset>JBOD_BLOCK_SIZE)
			{
				bytes_to_be_read = JBOD_BLOCK_SIZE-offset;
			}
			else{
				bytes_to_be_read = rem;
			}
			
			memcpy(read_buf+bytes_read,temp_buff+offset,bytes_to_be_read);
			bytes_read+=bytes_to_be_read;
			rem-=bytes_to_be_read;
		}
		else{
			if(rem > JBOD_BLOCK_SIZE){
			bytes_to_be_read=JBOD_BLOCK_SIZE;
			}
			else
			{
				bytes_to_be_read = rem;
			}
			memcpy(read_buf + bytes_read,temp_buff, bytes_to_be_read);
			bytes_read+=bytes_to_be_read;
			rem-=bytes_to_be_read;
		}
		start_addr += bytes_to_be_read;
}
return read_len;
}
//write starts
int mdadm_write(uint32_t start_addr, uint32_t write_len, const uint8_t *write_buf) {
	if(start_addr + write_len > JBOD_NUM_DISKS*JBOD_DISK_SIZE){
		return -1;
	}
	if (write_len>1024){
		return -1;
}
	if(is_mounted!=1){
		return -1;
	}
	if(write_buf == NULL && write_len != 0){
		return -1;
	}
	int bytes_wrote = 0;
	uint8_t temp_buff[JBOD_BLOCK_SIZE];
	int BlockID = 0; 
	int DiskID = 0; 
	int offset =0;

	int remm=write_len;
	uint32_t bytes_to_write;
	while (bytes_wrote < write_len){
		
		DiskID = start_addr/JBOD_DISK_SIZE;
		BlockID = start_addr%JBOD_DISK_SIZE / JBOD_NUM_BLOCKS_PER_DISK;
		offset = start_addr%JBOD_BLOCK_SIZE;


		uint32_t op = pack_bytes(DiskID,0,JBOD_SEEK_TO_DISK,0);
		if (jbod_client_operation(op,NULL) != 0) return -1;


		op = pack_bytes(0,BlockID,JBOD_SEEK_TO_BLOCK,0);
		if(jbod_client_operation(op,NULL) != 0) return -1;

		op = pack_bytes(0,0,JBOD_READ_BLOCK,0);
		if(jbod_client_operation(op,temp_buff) != 0) return -1;

		op = pack_bytes(DiskID,0,JBOD_SEEK_TO_DISK,0);
		if (jbod_client_operation(op,NULL) != 0) return -1;

		op = pack_bytes(0,BlockID,JBOD_SEEK_TO_BLOCK,0);
		if(jbod_client_operation(op,NULL) != 0) return -1;

		
		if(bytes_wrote==0){
			if(remm+offset>JBOD_BLOCK_SIZE)
			{
				bytes_to_write = JBOD_BLOCK_SIZE-offset;
			}
			else{
				bytes_to_write = remm;
			}
			
			memcpy(temp_buff+offset,write_buf,bytes_to_write);
			op = pack_bytes(0,0,JBOD_WRITE_BLOCK,0);
			if(jbod_client_operation(op,temp_buff) != 0) return -1;

			bytes_wrote+=bytes_to_write;
			remm-=bytes_to_write;
		}
		else{
			if(remm > JBOD_BLOCK_SIZE){
			bytes_to_write=JBOD_BLOCK_SIZE;
			}
			else
			{
				bytes_to_write = remm;
			}
			memcpy(temp_buff ,write_buf +bytes_wrote, bytes_to_write);
			op = pack_bytes(0,0,JBOD_WRITE_BLOCK,0);
			if(jbod_client_operation(op,temp_buff) != 0) return -1;

			bytes_wrote+=bytes_to_write;
			remm-=bytes_to_write;
		}
		start_addr += bytes_to_write;
}


	return write_len;
}


