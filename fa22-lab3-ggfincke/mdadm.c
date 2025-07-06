#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mdadm.h"
#include "jbod.h"

//op create
uint32_t op_create (uint32_t blockID, uint32_t diskID, uint32_t cmd, uint32_t reserved)
{
  //create return u32int_t that returns the combined values from passed vals 
  uint32_t retval = 0x0, tempa, tempb, tempc, tempd;
  tempa = blockID;
  tempb = diskID << 8;
  tempc = cmd << 12;
  tempd = reserved << 18;
  retval = tempa|tempb|tempc|tempd;

  //returns spliced op
  return retval;
  
}

//variables is mounted and is written for use in function
int is_mounted = 0;
int is_written = 0;

//mount
int mdadm_mount(void) 
{
  //if mounted fail
  if (is_mounted == 1)
    {
      return -1;
    }
  else
    {
      //op for mounting
      uint32_t op = op_create(0,0,JBOD_MOUNT, 0);
      //jbod operation will ignore everything if it sees JBOD_MOUNT
      jbod_operation(op, NULL);
      //set mounted to True
      is_mounted = 1;
    return 1;
    }
}

//unmount
int mdadm_unmount(void)
{
  //if not mounted fail
  if (is_mounted == 0)
    {
      return -1;
    }
  else
    {
      //op for unmounting
      uint32_t op = op_create(0,0,JBOD_WRITE_PERMISSION,0);
      //jbod operation will ignore everything if it sees JBOD_UNMOUNT
      jbod_operation(op, NULL);
      //mounted to False
      is_mounted = 0;
      return 1;
    }
}

//grant write permission
int mdadm_write_permission(void)
{
  //if already written then fail
  if (is_written == 1)
    {
      return -1;
    }
  else
    {
      //op for granting perm
      uint32_t op = op_create(0,0,JBOD_WRITE_PERMISSION,0);
      //jbod operation will ignore everything if it sees JBOD_WRITE_PERMISSION
      jbod_operation(op, NULL);
      //is written is true
      is_written = 1;
      return 0;
    }
}

//revoke write perm 
int mdadm_revoke_write_permission(void)
{
  if (is_written == 0)
    {
      return -1;
    }
  else
    {
      //op for revoking perm
      uint32_t op = op_create(0,0,JBOD_REVOKE_WRITE_PERMISSION,0);
      //jbod operation will ignore everything if it sees JBOD_REVOKE_WRITE_PERMISSION
      jbod_operation(op, NULL);
      //is written to false
      is_written = 0;
      return 0;
    }
}

//read
int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)
{
  //read read_len into read_buf starting at start_addr
  //read from an out of bound linear address should fail
  //a read larger than 2048 bytes should fail (2048 < read_len should fail); also means you can't read more than 8 blocks
  //there are multiple cases; reading a partial block, full block, and multiple blocks (also partial -> multiple -> partial)
  //getting read_buf can't be NULL but also must be able to accept a 0 read_len
  if (start_addr > 0x100000 || read_len > 0x800 || is_mounted == 0 || (read_buf == NULL && read_len != 0) || (read_len + start_addr) > 0x100000)
    {
      return -1;
    }

  else
    {
      //if read_len is 0 just return 0
      if (read_len == 0)
	{
	  return 0;
	}

      else
	{
	  //currentAddress, diskID, blockID, byteInBlock to determine if starting at arbitrary byte
	  //temp = start_addr
	  uint32_t temp = start_addr;
	  //floor division sets diskID to correct number
	  uint32_t diskID = temp/65536;
	  //blockID also uses floor division
	  uint32_t blockID = ((temp-(diskID*65536))/256);
	  //byteInBlock is the starting byte inside a block (found from the starting address and calculated diskID and blockID) 
	  uint32_t byteInBlock = temp - ((diskID*65536) + (blockID *256));
	    	 
	  //sets diskID and blockID to the internal i/o
	  uint32_t seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
	  uint32_t seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
	  //completed checks the number of bytes that was completed in the function compared to read_len
	  uint32_t completed = 0;
	  //temp buffer
	  uint8_t temp_buf[256];
	  uint32_t remaining = read_len;
	  //seeking before while loop
	  jbod_operation(seekDiskOp,NULL);
	  jbod_operation(seekBlockOp, NULL);

	  
	  //while completed bytes is less than the read length
	  //not checking middle length; only checking start and end 
	  while (completed < read_len)
	    {
	      //creating readBlock at the start
	      uint32_t readBlock = op_create(0, 0, JBOD_READ_BLOCK, 0);
	      //if the byteinblock isnt equal to 0 (if starting at an arbitrary byte)
	      if (byteInBlock != 0)
		{
		  //copy part of write_buf into temp_buf (depends on how long read_len is)
		  jbod_operation(readBlock, temp_buf);
		  memcpy(read_buf, &temp_buf[byteInBlock], 256-byteInBlock);

		  //copy only parts of the memory into 
		  remaining -= (256-byteInBlock);
		  completed += (256-byteInBlock);
		  byteInBlock = 0;
		  blockID++;

		  //if blockID is greater than 255, then we need to switch disks
		  if (blockID > 0xff)
		    {
		      diskID++;
		      blockID = 0;
		      seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
		    }
		  seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
		  jbod_operation(seekDiskOp,NULL);
		  jbod_operation(seekBlockOp, NULL); 
		}
	      
	      //if remaining is over 256 bytes (over 1 block), then an entire block is read
	      else if (remaining > 0xff) //>256
		{
		  //jbod operation read into temp_buf and then copy into read_buf
		  jbod_operation(readBlock, temp_buf);
		  memcpy(&read_buf[completed], temp_buf, 256);
		  completed += 256;
		  remaining -= 256;
		  blockID++;

		  //if blockID is greater than 255, then we need to switch disks
		  if (blockID > 0xff)
		    {
		      diskID++;
		      blockID = 0;
		      seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
		    }
		  seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
		  jbod_operation(seekDiskOp,NULL);
		  jbod_operation(seekBlockOp, NULL); 
		}

	      //else copy remaining memory into read_buf and finish (if ending on a partial block, it will always finish at the end)
	      else
		{
		  jbod_operation(readBlock, temp_buf);
		  memcpy(&read_buf[completed], &temp_buf[0], remaining);
		  completed += remaining;
		  remaining = 0;
		}    
	    }
	  //return read_len
	  return read_len;
	}
    }
}


int mdadm_write(uint32_t start_addr, uint32_t write_len, const uint8_t *write_buf)
{
  //wrtie from *write_buf into write_len starting at start_addr
  //write to an out of bound linear address should fail
  //a write larger than 2048 bytes should fail (2048 < write_len should fail); also means you can't write more than 8 blocks
  //there are multiple cases; reading a partial block, full block, and multiple blocks (also partial -> multiple -> partial)
  //getting read_buf can't be NULL but also must be able to accept a 0 read_len
  if (start_addr > 0x100000 || write_len > 0x800 || is_mounted == 0 || is_written == 0 || (write_buf == NULL && write_len != 0) || (write_len + start_addr) > 0x100000)
    {
      return -1;
    }

  else
    {
      //if write_len is 0 just return 0
      if (write_len == 0)
	{
	  return 0;
	}

      else
	{
	  //currentAddress, diskID, blockID, byteInBlock to determine if starting at arbitrary byte
	  //temp = start_addr
	  uint32_t temp = start_addr;
	  //floor division sets diskID to correct number
	  uint32_t diskID = temp/65536;
	  //blockID also uses floor division
	  uint32_t blockID = ((temp-(diskID*65536))/256);
	  //byteInBlock is the starting byte inside a block (found from the starting address and calculated diskID and blockID) 
	  uint32_t byteInBlock = (temp - ((diskID*65536) + (blockID*256)));
	  //creates initial seekDiskOp and seekBlockOp to the internal i/o
	  uint32_t seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
	  uint32_t seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
	  //completed checks the number of bytes that was completed in the function compared to write_len
	  uint32_t completed = 0;
	  //temp buffer; in write we are going to write into the temp buffer and then write the temp buffer to the block to be overidden (using JBOD write)
	  uint8_t temp_buf[256];
	  uint32_t remaining = write_len;
	 
 	  
	  //while completed bytes is less than the read length
	  while (completed < write_len)
	    {
	      //reading block into temp_buf and seeking back to the expected block (bc read increments by 1)
	      mdadm_read(((diskID * 65536) + ((blockID) * 256)), 256, temp_buf);
	      seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
	      seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
	      jbod_operation(seekDiskOp, NULL);
	      jbod_operation(seekBlockOp, NULL);
	      //creating op writeBlock
	      uint32_t writeBlock = op_create(0, 0, JBOD_WRITE_BLOCK, 0); 
	      
	      //if the byteinblock isnt equal to 0 (if starting at an arbitrary byte)
	      if (byteInBlock != 0)
		{
		  //copy part of write_buf into temp_buf (depends on how long write_len is)
		  if (write_len > (256-byteInBlock))
		    {
		      memcpy(&temp_buf[byteInBlock], write_buf, (256-byteInBlock));
		    }
		  else
		    {
		      memcpy(&temp_buf[byteInBlock], write_buf, write_len);
		    }
		  //adjusting parameters and writing the block with temp_buf				   
		  remaining -= (256-byteInBlock);
		  completed += (256-byteInBlock);
		  byteInBlock = 0;
		  blockID++;
		  jbod_operation(writeBlock, temp_buf);
		  //if blockID is greater than 255 then we need to switch disks
		  if (blockID > 0xff)
		    {
		      diskID++;
		      blockID = 0;
		      seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
		    }
		  //creating seekBlock op for new disk
		  seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
		  jbod_operation(seekDiskOp,NULL);
		  jbod_operation(seekBlockOp, NULL);
		}	      

	      //if remaining is over 256 bytes (over 1 block)
	      else if (remaining > 0x100) //>256
		{
		  //copies write_buf from position of completed into the entirety of temp_buf
		  memcpy(temp_buf, &write_buf[completed], 256);
		  completed += 256;
		  remaining -= 256;
		  blockID++;
		  jbod_operation(writeBlock, temp_buf);
		  //if blockID > 255 then switch disks (same as before)
		  if (blockID > 0xff)
		    {
		      diskID++;
		      blockID = 0;
		      seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
		    }
		  //creating seekBlock op for new disk
		  seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
		  jbod_operation(seekDiskOp,NULL);
		  jbod_operation(seekBlockOp, NULL);
		}

	      //else copy remaining memory into temp_buf and finish (if ending on a partial block, it will always finish at the end)
	      else
		{
		  memcpy(temp_buf, &write_buf[completed], remaining);
		  completed += remaining;
		  remaining = 0;
		  jbod_operation(writeBlock, temp_buf);
		}
	      
	    }
	  //return read_len
	  return write_len;
	}    
    }
}


