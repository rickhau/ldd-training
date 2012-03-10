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
#define VERSION 6 

#define BUF_SIZE (10000)
#define LCD_SIZE (320*240*4)

struct cdata_t {
  unsigned long *fb;
  unsigned char *buf;
  unsigned int index;
  unsigned int offset;
  struct timer_list flush_timer;
  struct timer_list sched_timer;
  //DECLARE_WAIT_QUEUE(wq);
  wait_queue_head_t wq;
};


static int cdata_open(struct inode *inode, struct file *filp)
{
	int minor;
	struct cdata_t *cdata;
   	printk(KERN_INFO "CDATA: In OPEN.\n");
	minor = MINOR(inode->i_rdev);
	printk(KERN_INFO "CDATA: Minor number: %d\n", minor);

	cdata = kmalloc(sizeof(struct cdata_t), GFP_KERNEL);
	cdata->buf = kmalloc(BUF_SIZE, GFP_KERNEL);
	cdata->fb = ioremap(0x33f00000, 320*240*4);
	cdata->index = 0;
	cdata->offset = 0;

	init_timer(&cdata->flush_timer);
	init_timer(&cdata->sched_timer);

	init_waitqueue_head(&cdata->wq);

	filp->private_data = (void *)cdata;
	return 0;
}

static ssize_t cdata_read(struct file *filp, char *buff, size_t size, loff_t *off)
{
	return 0;
}

void flush_lcd(unsigned long *priv)
{
  	struct cdata_t *cdata = (struct cdata_t *)priv;
	unsigned char *fb;
	unsigned char *pixel;
	int index;
	int offset;
	int i;

	fb = (unsigned char*)cdata->fb;
	pixel = cdata->buf;
	index = cdata->index;
	offset = cdata->offset;

	for( i = 0; i < index; i++ ){
	  writeb(pixel[i], fb+offset);
	  offset++;
	  if(offset >= LCD_SIZE)
	    offset = 0;

	}
	cdata->index = 0;
	cdata->offset = offset;
}

void cdata_wake_up(unsigned long priv)
{
   // Wake up process (Switch process to ready)
   struct cdata_t *cdata = (struct cdata_t *)priv;
   struct timer_list *sched;
   wait_queue_head_t *wq;

   sched = &cdata->sched_timer;
   wq = &cdata->wq;

   // because kernel time expire belongs to I/O interrupt, 
   // You CAN NOT switch process state in I/O interrupt
   // Also, No context switching in I/O interrupt as well
   //current->state = TASK_RUNNING;
   //schedule();
   wake_up(wq);

   sched->expires = jiffies + 10;
   add_timer(sched);
}

static ssize_t cdata_write(struct file *filp, const char *buf, size_t size, loff_t *off)
{
  	struct cdata_t *cdata = (struct cdata_t *)filp->private_data;
	unsigned char *pixel;
	unsigned int i;
	unsigned int index;
	struct timer_list *timer;
	struct timer_list *sched;
	wait_queue_head_t *wq;
	wait_queue_t wait;
	
	pixel = cdata->buf;
	index = cdata->index;
	timer = &cdata->flush_timer;
	sched = &cdata->sched_timer;
	wq = &cdata->wq;
	//printk(KERN_INFO "CDATA: In cdata_write()\n");
        	
	for (i = 0; i < size; i++){
	  if (index >= BUF_SIZE){

	     cdata->index = index;
	     // Kernel scheduling
	     timer->expires = jiffies + 5*HZ;  // 1*HZ = 1  second
	     timer->function = flush_lcd;
	     timer->data = (unsigned long)cdata;
	     add_timer(timer);

	     // You can setup one timer(timer) without sched
	     // Timer expires, call flush_lcd() 
	     // When flush_lcd() finish the write I/O, then wake up the process
	     // Then you do notn need to maintain 2nd timer for process state change
	     sched->expires = jiffies + 10;  // 10 == 0.1 second
	     sched->function = cdata_wake_up;
	     sched->data = (unsigned long)cdata;
	     add_timer(sched);

	     wait.flags = 0;
	     wait.task = current;
	     add_wait_queue(wq,&wait); 
repeat:

	     // Process scheduling
	     current->state = TASK_INTERRUPTIBLE;
	     schedule();

	     // Every time, sched timer expires, it will read the index and check if index != 0 
	     // Meaning flush_lcd has not finished yet. So, keep changing itself to ready state for next wake up.
	     index = cdata->index;   // IMPORTANT: Use state machine concept to maintain. Do not use index = 0; not good!

	     if (index != 0)
		goto repeat;

	     remove_wait_queue(wq, &wait);
	     del_timer(sched);
          }

	  //fb[index] = buf[i];  // wrong!! Can NOT access user space data directly
	  copy_from_user(&pixel[index], &buf[i], 1);
	  index++;
	}
	cdata->index = index;
        //while(1) {
	  //current->state=TASK_UNINTERRUPTIBLE;
	//  current->state=TASK_INTERRUPTIBLE;
	//  schedule();
	//}
	return 0;
}

static int cdata_close(struct inode *inode, struct file *filp)
{
  	struct cdata_t* cdata = (struct cdata_t *)filp->private_data;
	printk(KERN_INFO "CDATA: cdata_close() is invoked.\n"); 

	flush_lcd((unsigned long)cdata);

	del_timer(&cdata->flush_timer);

	kfree(cdata->buf);
	kfree(cdata);
	return 0;
}

static int cdata_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
  int i,n ;
  unsigned long *fb;
  struct cdata_t *cdata = (struct cdata_t *)filp->private_data;

  switch (cmd) {
    case CDATA_CLEAR:
       //n = *((int *) arg); // dirty
       copy_from_user(&n, (int*)arg, 4);
       //get_user(n,(int*)arg);
       printk(KERN_INFO "Action: CDATA_CLEAR: %d pixel.\n", n);
       //fb = ioremap(0x33f00000, n*4);  // FIXME: dirty

       // fork & multithread needs to file locking on fb
       // FIXME: Lock
       fb = cdata->fb;
       // FIXME: unlock
       for( i = 0; i < n; i++)
         writel(0x00ff00ff, fb++);  
       return 0;
    case CDATA_RED:
       break;
    case CDATA_GREEN:
       break;
    case CDATA_BLUE:
       break;
    case CDATA_BLACK:
       break;
    case CDATA_WHITE:
       break;
  }
  return -ENOTTY;
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
  unsigned long *fb;
  int i;
  fb = ioremap(0x33f00000, 320*240*4);
  for( i = 0; i<320*240; i++)
    writel(0x00ff0000, fb++);
  printk(KERN_INFO "CDATA: Exercise [ %d ]\n", VERSION);
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
