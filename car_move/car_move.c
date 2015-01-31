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
#include <plat/map-base.h>


#define DEVICE_NAME "SetPin"
static struct semaphore lock;
struct cdev *setpin_cdev;
unsigned int counter=0;
unsigned long tmp;


static int setpin_open(struct inode *inode,struct file *filp)
{  
if(!down_trylock(&lock))
return 0; 
else
	return -EBUSY;
}



static long setpin_ioctl( struct file *file, unsigned int cmd, unsigned long arg)
{

	switch (cmd)
		{
		case 0:
			gpio_set_value(S5PV210_GPH2(1),1);
			gpio_set_value(S5PV210_GPH2(2),0);
			gpio_set_value(S5PV210_GPH3(0),1);
                        gpio_set_value(S5PV210_GPH3(1),0);

			break;
		case 1:
			gpio_set_value(S5PV210_GPH2(1),0);
                        gpio_set_value(S5PV210_GPH2(2),1);
			gpio_set_value(S5PV210_GPH3(0),0);
                        gpio_set_value(S5PV210_GPH3(1),1);

			break;
		case 5:
			gpio_set_value(S5PV210_GPH2(1),1);
                        gpio_set_value(S5PV210_GPH2(2),0);
                        gpio_set_value(S5PV210_GPH3(0),0);
                        gpio_set_value(S5PV210_GPH3(1),1);
			break;
		case 3:
			gpio_set_value(S5PV210_GPH2(1),0);
                        gpio_set_value(S5PV210_GPH2(2),1);
                        gpio_set_value(S5PV210_GPH3(0),1);
                        gpio_set_value(S5PV210_GPH3(1),0);
			break;
		case 4:
			gpio_set_value(S5PV210_GPH2(1),0);
                        gpio_set_value(S5PV210_GPH2(2),0);
                        gpio_set_value(S5PV210_GPH3(0),0);
                        gpio_set_value(S5PV210_GPH3(1),0);
			break;



		default:
			return -EINVAL;	
		}
		return 0;
}

static int setpin_close(struct inode *inode,struct file *filp)
{

 up(&lock);
return 0;
}

static struct file_operations setpin_fops={
.owner=THIS_MODULE,
.open=setpin_open,
.unlocked_ioctl=setpin_ioctl,//从内核2.6开始需要加上unlocked_
.release=setpin_close,
};

static struct miscdevice misc = {
.minor = MISC_DYNAMIC_MINOR,
.name = DEVICE_NAME,
.fops = &setpin_fops,
};

static int __init setpin_init(void)
{
	int ret;
	sema_init(&lock,1);
	ret = misc_register(&misc);
	printk (DEVICE_NAME"/tinitialized/n");

	s3c_gpio_setpull(S5PV210_GPH2(1),S3C_GPIO_PULL_UP);
	 s3c_gpio_setpull(S5PV210_GPH2(2),S3C_GPIO_PULL_UP);

        s3c_gpio_cfgpin(S5PV210_GPH2(1),S3C_GPIO_OUTPUT); //设置GPh21口输出
	s3c_gpio_cfgpin(S5PV210_GPH2(2),S3C_GPIO_OUTPUT); //设置GPh21口输出
	 s3c_gpio_setpull(S5PV210_GPH3(0),S3C_GPIO_PULL_UP);
         s3c_gpio_setpull(S5PV210_GPH3(1),S3C_GPIO_PULL_UP);

        s3c_gpio_cfgpin(S5PV210_GPH3(0),S3C_GPIO_OUTPUT); //设置GPh21口>输出
        s3c_gpio_cfgpin(S5PV210_GPH3(1),S3C_GPIO_OUTPUT); //设置GPh21口>输出

if(ret<0){
	printk("Register SETPIN failed!/n");
	return ret;
	}
printk("Register iopwm success!/n");
return ret;
}

static void __exit setpin_exit(void)
{
misc_deregister(&misc);
}

module_init(setpin_init);
module_exit(setpin_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("wangside");
