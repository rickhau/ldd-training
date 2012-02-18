#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <linux/input.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "cdata_ioctl.h"

#define DEV_MAJOR 121
#define DEV_NAME "cdata"

//struct cdata_t {
//  unsigned long *fb;
//}


static int cdata_open(struct inode *inode, struct file *filp)
{
	int minor;
   	printk(KERN_INFO "CDATA: In OPEN.\n");
	minor = MINOR(inode->i_rdev);
	printk(KERN_INFO "CDATA: Minor number: %d\n", minor);

	//cdata = kmalloc(sizeof(struct cdata_t), GFP_KERNEL);
	//cdata -> fb = ioremap(0x33ff00000, 320*24*4);
	return 0;
}

static int cdata_close(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "CDATA: cdata_close() is invoked.\n"); 
	return 0;
}

static ssize_t cdata_read(struct file *filp, char *buff, size_t size, loff_t *off)
{
	return 0;
}

static ssize_t cdata_write(struct file *filp, const char *buff, size_t size, loff_t *off)
{
	//unsigned int i;
        while(1) {
	  //current->state=TASK_UNINTERRUPTIBLE;
	  current->state=TASK_INTERRUPTIBLE;
	  schedule();
	}
	return 0;
}

static int cdata_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
  int i,n ;
  unsigned long *fb;
  n = *((int *) arg); // FIXME
  switch (cmd) {
    case CDATA_CLEAR:
       printk(KERN_INFO "Action: CDATA_CLEAR: %d pixel.\n", n);
       fb = ioremap(0x33f00000, n*4);
       for( i = 0; i<n; i++)
         writel(0x00ff0000, fb++);
       break;
  }
  return 0;
}

static int cdata_flush(struct file *filp)
{
	printk(KERN_INFO "CDATA: cdata_flush() called back!!\n");
	return 0;
}

static struct file_operations cdata_fops = {	
	owner:		THIS_MODULE,
	open:		cdata_open,
	release:	cdata_close,
	read:		cdata_read,
	write:		cdata_write,
	ioctl:		cdata_ioctl,
	flush:		cdata_flush,
};

static int cdata_init_module(void)
{
  //unsigned long *fb;
  //int i;
  //fb = ioremap(0x33f00000, 320*240*4);
  //for( i = 0; i<320*240; i++)
  //  writel(0x00ff0000, fb++);

  if(register_chrdev(DEV_MAJOR, DEV_NAME, &cdata_fops) < 0){
   printk(KERN_INFO "CDATA: Couldn't register a device.\n");
   return -1; 
  }
  printk(KERN_INFO "CDATA: In cdata_init_module.\n");
  return 0;
}

static void cdata_cleanup_module(void)
{
  unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

module_init(cdata_init_module);
module_exit(cdata_cleanup_module);

MODULE_LICENSE("GPL");
