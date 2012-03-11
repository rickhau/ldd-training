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
#include "cdata_ts_ioctl.h"

#define DEV_MAJOR 121
#define DEV_NAME "cdata"
#define VERSION 1 

static int cdata_ts_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t cdata_ts_read(struct file *filp, char *buff, size_t size, loff_t *off)
{
	return 0;
}

static ssize_t cdata_ts_write(struct file *filp, const char *buf, size_t size, loff_t *off)
{
	return 0;
}

static int cdata_ts_close(struct inode *inode, struct file *filp)
{
	return 0;
}

static int cdata_ts_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	return -ENOTTY;
}

static struct file_operations cdata_ts_fops = {	
	owner:		THIS_MODULE,
	open:		cdata_ts_open,
	release:	cdata_ts_close,
	read:		cdata_ts_read,
	write:		cdata_ts_write,
	ioctl:		cdata_ts_ioctl,
};

static int cdata_ts_init_module(void)
{
  printk(KERN_INFO "CDATA: Exercise [ %d ]\n", VERSION);
  if(register_chrdev(DEV_MAJOR, DEV_NAME, &cdata_ts_fops) < 0){
   printk(KERN_INFO "CDATA: Couldn't register a device.\n");
   return -1; 
  }
  printk(KERN_INFO "CDATA: In cdata_ts_init_module.\n");
  return 0;
}

static void cdata_ts_cleanup_module(void)
{
  unregister_chrdev(DEV_MAJOR, DEV_NAME);
}

module_init(cdata_ts_init_module);
module_exit(cdata_ts_cleanup_module);

MODULE_LICENSE("GPL");
