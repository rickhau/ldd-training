// test.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

int main(void)
{
  int fd;
  fd = open("/dev/cdata3", O_RDWR);
  fd = open("/dev/cdata3", O_RDWR);
  //fd = open("/dev/cdata", O_RDONLY);
  sleep(10);
  close(fd);
  close(fd);
}
