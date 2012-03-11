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

#define CDATA_TS_MINOR 2

void cdata_bh(unsigned long);
DECLARE_TASKLET(my_tasklet, cdata_bh, NULL);

struct input_device ts_input;

void cdata_ts_handler(struct file *filp, struct pt_regs *reg)
{
	printk(KERN_INFO "data_ts: TH....\n");
	// FIXME: read(x,y) from ADC. To Read HW data referring to datasheet
	tasklet_schedule(&my_tasklet);
}

void cdata_bh(unsigned long priv)
{
	printk(KERN_INFO "data_ts: BH down....\n");
}

static int cdata_ts_open(struct inode *inode, struct file *filp)
{
	//u32 reg;
#if 0
	// This coding style is hard to read in the future
	// Using set_gpio_ctrl macro provided by chipset vendor is better to read
	reg = GPGCON; // Read value from GPGCON register
	reg |= 0xff000000; // To set 4 pins to 1 , same as below 4 macro did
	GPGCON = reg;
#endif
	//printk(KERN_INFO "GPGCON: %08x\n", reg);

	set_gpio_ctrl(GPIO_YPON);
	set_gpio_ctrl(GPIO_YMON);
	set_gpio_ctrl(GPIO_XPON);
	set_gpio_ctrl(GPIO_XMON);

	ADCTSC = DOWN_INT | XP_PULL_UP_EN | \
		 XP_AIN | XM_HIZ | YP_AIN | YM_GND | \
		 XP_PST(WAIT_INT_MODE);

	// Request touch panel IRQ 
	if(request_irq(IRQ_TC, cdata_ts_handler, 0, "cdata-ts", 0)) {
		printk(KERN_ALERT "cdata: request irq failed.\n");
		return -1;
	}

	// handler input device
	ts_input.open = 
	ts_input.close =

	input_register_device(&ts_input);

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


static struct miscdevice cdata_ts_misc = {
	minor: 		CDATA_TS_MINOR,
	name:		"cdata-ts",
	fops:		&cdata_ts_fops,
};

static int cdata_ts_init_module(void)
{
  if(misc_register(&cdata_ts_misc) < 0){
   printk(KERN_INFO "CDATA-TS: Couldn't register a driver.\n");
   return -1; 
  }
  printk(KERN_INFO "CDATA-TS: In cdata_ts_init_module.\n");
  return 0;
}

static void cdata_ts_cleanup_module(void)
{
  misc_deregister(&cdata_ts_misc);
}

module_init(cdata_ts_init_module);
module_exit(cdata_ts_cleanup_module);

MODULE_LICENSE("GPL");
