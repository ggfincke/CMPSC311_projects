//CMPSC 311 FA22
//LAB 2

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "mdadm.h"
#include "jbod.h"

//method op create to create the operation 
uint32_t op_create (uint32_t blockID, uint32_t diskID, uint32_t cmd, uint32_t reserved)
{
  //create return u32int_t that returns the combined values from passed vals 
  uint32_t retval = 0x0, tempa, tempb, tempc, tempd;
  tempa = blockID;
  tempb = diskID << 8;
  tempc = cmd << 12;
  tempd = reserved << 18;
  retval = tempa|tempb|tempc|tempd;

  return retval;
  
}

//int mounted that is modified to determine if mounted or not
int mounted = 0;
//mount function
int mdadm_mount(void)
{
  //if mounted fail
  if (mounted == 1)
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
      mounted = 1;
    return 1;
  }
}

//unmount function
int mdadm_unmount(void)
{
  //if not mounted fail
  if (mounted == 0)
    {
      return -1;
    }
  else
    {
      //op for unmounting
      uint32_t op = op_create(0,0,JBOD_UNMOUNT,0);
      //jbod operation will ignore everything if it sees JBOD_UNMOUNT
      jbod_operation(op, NULL);
      //mounted to False
      mounted = 0;
      return 1;
    }
}

//read function
int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)
{
  //read read_len into read_buf starting at start_addr
  //read from an out of bound linear address should fail
  //a read larger than 2048 bytes should fail (2048 < read_len should fail); also means you can't read more than 8 blocks
  //there are multiple cases; reading a partial block, full block, and multiple blocks (also partial -> multiple -> partial)
  //getting read_buf can't be NULL but also must be able to accept a 0 read_len
  if (start_addr > 0x100000 || read_len > 0x800 || mounted == 0 || (read_buf == NULL && read_len != 0) || (read_len + start_addr) > 0x100000)
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
	  //byteinblock has different cases for if blockID is 0 because that will effect the math
	  uint32_t byteInBlock = 0;
	  if (blockID == 0)
	    {
	      byteInBlock = (temp-(diskID*65536));
	    }
	  else
	    {
	      byteInBlock = (256-(((diskID+1)*65536)-temp));
	    }
	 
	  //sets diskID and blockID to the internal i/o
	  uint32_t seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
	  uint32_t seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
	  //completed checks the number of bytes that was completed in the function compared to read_len
	  uint32_t completed = 0;
	  //temp buffer
	  uint8_t temp_buf[256];
	  uint32_t remaining = read_len;
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
		  
		  jbod_operation(readBlock, temp_buf);
		  //copy only parts of the memory into 
		  memcpy(&read_buf[0], &temp_buf[byteInBlock], 256-byteInBlock);
		  remaining -= (256-byteInBlock);
		  completed += (256-byteInBlock);
		  blockID++;
		  uint32_t seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
		  jbod_operation(seekBlockOp, NULL);
		  byteInBlock = 0;

		  //if blockID is greater than 255 then we need to switch disks
		  if (blockID > 0xff)
		    {
		      diskID++;
		      blockID = 0;
		      uint32_t seekDiskOp = op_create(0, diskID, JBOD_SEEK_TO_DISK, 0);
		      jbod_operation(seekDiskOp,NULL);
		    }
		}	      

	      //if remaining is over 256 bytes (over 1 block)
	      else if (remaining > 0x100) //>256
		{
		  //if blockID > 255 then performs jbod read operation and switches diskID??
		  jbod_operation(readBlock, temp_buf);
		  memcpy(&read_buf[completed], temp_buf, 256);
		  completed += 256;
		  remaining -= 256;
		  blockID++;
		  uint32_t seekBlockOp = op_create(blockID, 0, JBOD_SEEK_TO_BLOCK, 0);
		  jbod_operation(seekBlockOp, NULL);
		}

	      //else copy remaining memory into read_buf and finish (if there is a partial block it will always be the end)
	      else
		{
		 
		  jbod_operation(readBlock, temp_buf);
		  memcpy(&read_buf[completed], temp_buf, remaining);
		  completed += remaining;
		  remaining = 0;
		  
		}
	      
	    }
	  //return read_len
	  return read_len;
	}    
    }
}

