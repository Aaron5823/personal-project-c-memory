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

/* the client socket descriptor for the connection to the server */
int cli_sd = -1;

/* attempts to read n bytes from fd; returns true on success and false on
 * failure */
bool nread(int fd, int len, uint8_t *buf) {
int bytes_read = 0;
int bytes_to_be_read = 0;

  while(bytes_read < len){
    //printf("hello");
    fflush(stdout);
    bytes_to_be_read = read(fd,&buf[bytes_read], len - bytes_read);
  if(bytes_to_be_read <= 0){
    return false;
  }
  
  bytes_read += bytes_to_be_read;
  }
  
  return true;
}

/* attempts to write n bytes to fd; returns true on success and false on
 * failure */
bool nwrite(int fd, int len, uint8_t *buf) {

  int bytes_wrote = 0;
  int bytes_to_write = 0;
  while(bytes_wrote < len){
    //printf("sd");
    fflush(stdout);
    bytes_to_write = write(fd,&buf[bytes_wrote], len - bytes_wrote);
    if(bytes_to_write <= 0){
      return false;
    }
    bytes_wrote += bytes_to_write;
  }
  return true;
}

/* attempts to receive a packet from fd; returns true on success and false on
 * failure */
bool recv_packet(int fd, uint32_t *op, uint8_t *ret, uint8_t *block) {
    uint8_t packet[HEADER_LEN];
    

    if (!nread(fd, HEADER_LEN, packet)){
      return false;
    }
    memcpy(op, packet, sizeof(uint32_t));

    *op = ntohl(*op);
    memcpy(ret, packet + sizeof(uint32_t), sizeof(uint8_t));

    if((*ret &2) == 2){
      nread(fd,JBOD_BLOCK_SIZE, block);
    }

    return true;

}




/* attempts to send a packet to sd; returns true on success and false on
 * failure */
bool send_packet(int sd, uint32_t op, uint8_t *block) {
  uint32_t opPtr = htonl(op);
  uint8_t jbodPacket[HEADER_LEN + JBOD_BLOCK_SIZE];
  int header_len = HEADER_LEN;
  int packetLen = header_len;

  memcpy(jbodPacket, &opPtr, sizeof(uint32_t));



  if (op >> 12 == JBOD_WRITE_BLOCK) {
    packetLen += JBOD_BLOCK_SIZE; 
    memcpy (jbodPacket + HEADER_LEN, block, JBOD_BLOCK_SIZE); 
    jbodPacket [sizeof(uint32_t)]  = 2; 
  }
  else
  {
    jbodPacket[sizeof(uint32_t)] = 0;
  }
  //printf("jx");
  return nwrite(sd, packetLen, jbodPacket);
}

/* connect to server and set the global client variable to the socket */
bool jbod_connect(const char *ip, uint16_t port) {
  //printf("program is working\n");
  fflush(stdout);
  struct sockaddr_in caddr;
  caddr.sin_family = AF_INET;
  caddr.sin_port = htons(port);

  if(inet_aton(ip, &(caddr.sin_addr))==0){
    return false;
  }

  cli_sd = socket(AF_INET, SOCK_STREAM,0);
  if(cli_sd == -1){
    printf("error on socket creation [%s]\n", strerror(errno));
  }
  if(connect(cli_sd,(const struct sockaddr *)&caddr, sizeof(caddr))== -1){
      printf("error on socket connect [%s]\n", strerror(errno));
    return false;
  }
  return true;
}
void jbod_disconnect(void) {
  if(close(cli_sd)== -1){
    printf("failure to disconnect client from server");
  }
  cli_sd = -1;
}
  


int jbod_client_operation(uint32_t op, uint8_t *block) {
 // printf("\nHRTO\n");
  fflush(stdout);
  if(!send_packet(cli_sd,op,block))return -1;
  uint8_t ret;
  uint32_t return_op;
  if(!recv_packet(cli_sd, &return_op, &ret, block))return -1;
  if(return_op != op){

    printf("\nops dont match\n");
  }
  return 0;
}
