// test.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "cdata_ioctl.h"

int main(int argc, char **argv)
{
  int fd;
  int i;
  char pix[4] = { 0x00, 0xff, 0x00, 0xff };
  //fd = open("/dev/cdata3", O_RDWR);
  //fd = open("/dev/cdata3", O_RDWR);
  //fd = open("/dev/cdata", O_RDONLY);
  i = 10000;
  fd = open("/dev/cdata", O_RDWR); // WR: Write
  //printf("Going to Clear Screen...\n");
  //ioctl(fd, CDATA_CLEAR, &i); 
  printf("Going to write pixel...\n");
  while(1){
    write(fd, pix, 4);
  }
  //write(fd, pix, 4);
  //write(fd, "123", 3);
  //sleep(10);
  //close(fd);
  close(fd);
}
