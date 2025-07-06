#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "net.h"
#include "jbod.h"

//socket descriptor 
int cli_sd = -1;

//this nread function should be guaranteed to run for the passed length
static bool nread(int fd, int len, uint8_t *buf)
{
  //read len and count len
  int rLen = 0;
  int cLen = 0;
  //while read length is less than the passed length
  while (rLen < len)
  {
    //count length is equal to the bytes read 
    cLen = read(fd, &buf[cLen], len);
    if (cLen < 0)
    {
      return false;
    }
    //read length is incremented
    else
    {
      rLen += cLen;
    }   
  }
  //return true if runs as expected
  return true;
}


//nwrite needs to run for all passed len; runs exact same as nread
static bool nwrite(int fd, int len, uint8_t *buf)
{
  //wLen replaces rLen
  int wLen = 0;
  int cLen = 0;
  while (wLen < len)
  {
    //count len
    cLen = write(fd, &buf[cLen], len);
    if (cLen < 0)
    {
      return false;
    }
    //write length is incremented
    else
    {
      wLen += cLen;
    }   
  }
  //return true if runs as expected
  return true;
}

//receive packet receives info from socket 
static bool recv_packet(int sd, uint32_t *op, uint8_t *ret, uint8_t *block)
{
  //header is the first 5 bytes to be read
  uint8_t header[5];
  //if nread fails this must too
  if (nread(sd, 5, header) == false)
    {
      return false;
    }
  //infocode is the 5th byte
  uint8_t infocode = header[4];
  //splitting infocode to the last two bytes
  infocode = infocode << 6;
  //if shifted infocode has _1000000 then the JBOD operation failed and false should be returned 
  if (infocode == 64 || infocode == 192) 
  {
    return false;
  }
  //else then check if block needs returned as well
  else
    {
      memcpy(op, header, 4);
      memcpy(ret, &header[4], 1);
      //if shifted infocode is 00000000 then the JBOD operation succeeded with no data block
      if (infocode == 0) 
      {
	block = NULL;
	return true;
      }
      //if not returned then a block must be read
      if (nread(sd, 256, block) == false)
      {
	return false;
      }
      return true;
    }
}

//send packet sends packet to the socket
static bool send_packet(int sd, uint32_t op, uint8_t *block)
{
  //if block is NULL there is no block to write from
  if (block == NULL)
  {
    //create wrapper (header) to be 5 bytes
    uint8_t wrap[5];
    //htop is the network code for op
    uint32_t htOp;
    htOp = htonl(op);
    //copy into wrap
    memcpy(wrap, &htOp, 4);
    //bc no block, set infocode to 0
    wrap[4] = 00000000;
    //write to socket
    if (nwrite(sd, 5, wrap) == true)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  //else there is a block that needs to written from
  else
  {
    //total to be wrapped is 261 (5 + 256)
    uint8_t wrap[261];
    //network code
    uint32_t htOp;
    htOp = htonl(op);
    memcpy(wrap, &htOp, 4);
    //set infocode to denote a block needs to be read
    wrap[4] = 2;
    //memcpy block to wrapper
    memcpy(&wrap[5], &block[0], 256);
    //write to socket
    if (nwrite(sd, 261, wrap) == true)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

//connect to jbod_server via ip address and port
bool jbod_connect(const char *ip, uint16_t port)
{
  struct sockaddr_in addr;

  //socket
  cli_sd = socket(AF_INET, SOCK_STREAM, 0);
  //if socket failed
  if (cli_sd < 0)
  {
    return false;
  }

  //if inet_aton fails
  if (inet_aton(ip, &(addr.sin_addr)) < 1)
  {
    return false;
  }
  //setting other parts of addr
  addr.sin_port = htons(port);
  addr.sin_family = AF_INET;

  //connecting
  if (connect(cli_sd, (const struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
    return false;
  }
  //if no errors connecting then return true
  return true;		  
}

//disconnect
void jbod_disconnect(void)
{
  close(cli_sd);
  cli_sd = -1;
}

//jbod_clinet operation 
int jbod_client_operation(uint32_t op, uint8_t *block)
{
  //if sending fails return false
  if (send_packet(cli_sd, op, block) == false)
  {
    return -1;
  }

  //if receiving fails return false
  uint8_t ret = 0;
  if (recv_packet(cli_sd, &op, &ret, block) == false)
  {
    return -1;
  }

  //else then all worked and returns 0
  return 0;
  
}
