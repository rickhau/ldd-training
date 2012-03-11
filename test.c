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
  char *fb;
  int x;
  //int i;
#if 0
  char pix[4] = { 0x00, 0xff, 0x00, 0xff }; // GREEN
  char bpix[4] = { 0xff, 0x00, 0x00, 0x00 }; // BLUE 
  pid_t pid;
#endif
  //fd = open("/dev/cdata3", O_RDWR);
  //fd = open("/dev/cdata", O_RDONLY);
//  i = 10000;
  fd = open("/dev/cdata", O_RDWR); // WR: Write
//  printf("Going to fork()...\n");
//  ioctl(fd, CDATA_CLEAR, &i); 
#if  0
  pid = fork();
  //fd = open("/dev/cdata", O_RDWR); // WR: Write

  if ( pid == 0 ) {  // parent process
     // GREEN
    while(1){
      write(fd, pix, 4);
    }
  }else{ // child process
    // BLUE
    while(1){
      write(fd, bpix, 4);
    }
  }
#endif
  //while(1)
  //  write(fd, pix, 4);
  //write(fd, "123", 3);
  //sleep(10);
  //close(fd);
  fb = (char *)mmap(0, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  for( x = 0 ; x < 1024 ; x++ )
  {
	  *fb = 0x00; fb++;
	  *fb = 0xff; fb++;
	  *fb = 0x00; fb++;
	  *fb = 0x00; fb++;
  }
  //sleep(30);
  close(fd);
}
