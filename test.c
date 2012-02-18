// test.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
  int fd;
  int i;
  //fd = open("/dev/cdata3", O_RDWR);
  //fd = open("/dev/cdata3", O_RDWR);
  //fd = open("/dev/cdata", O_RDONLY);
  fd = open("/dev/cdata", O_RDONLY);
  ioctl(fd, argv[1], &i); 
  write(fd, "123", 3);
  //sleep(10);
  //close(fd);
  close(fd);
}
