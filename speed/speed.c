#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <plat/regs-timer.h>
#include <mach/regs-irq.h>
#include <asm/mach/time.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/timer.h>
#include <linux/irq.h>
#include <mach/map.h>
#include <linux/gpio.h>//用于gpio的那些函数
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#define DEVICE_NAME "speed"
#define speed_irq IRQ_EINT(27)
struct cdev *iopwm_cdev;
int counter;
//#define SDA S5PV210_GPH3(3)


static irqreturn_t speed_interrupt(int irq , void *dev_id)
{
	if(irq !=IRQ_EINT(27))
	{
	printk("bad irq % d in EINT27 /n", irq);
	return -1;
	}
	//if(gpio_get_value( S5PV210_GPH3(3))==1)
	counter++;
	return IRQ_RETVAL(IRQ_HANDLED);
}

static int speed_open(struct inode *inode,struct file *filp)
{  
	int ret;

	ret=request_irq(speed_irq,speed_interrupt,IRQ_TYPE_EDGE_RISING, DEVICE_NAME, NULL);
	if(ret<0){
printk("Register speed failed!/n");
return ret;
}
printk("Register speed success!/n");
return 0; 
}


static int speed_close(struct inode *inode,struct file *filp)
{
 free_irq(speed_irq,NULL);
return 0;
}

static ssize_t speed_read(struct file *filp,int __user *buff,size_t count,loff_t *offp)
{
		int err;
		//int num;
		//num=counter;
		//msleep(1000);
		//num=counter-num;
		err=copy_to_user(buff,&counter,count);
		if(err<0)
			printk("read fail");
		
		return count;
}

static struct file_operations speed_fops={
.owner=THIS_MODULE,
.open=speed_open,
.read=speed_read,
.release=speed_close,
};

static struct miscdevice misc = {
.minor = MISC_DYNAMIC_MINOR,
.name = DEVICE_NAME,
.fops = &speed_fops,
};

static int __init speed_init(void)
{
int ret;
s3c_gpio_cfgpin(S5PV210_GPH3(3),S3C_GPIO_INPUT); 
ret = misc_register(&misc);
printk (DEVICE_NAME"/tinitialized/n");


return ret;
}

static void __exit speed_exit(void)
{

misc_deregister(&misc);
}

module_init(speed_init);
module_exit(speed_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wangside");
