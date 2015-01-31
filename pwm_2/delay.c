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


#define DEVICE_NAME "iopwm"
#define iopwm_irq IRQ_TIMER1 
static struct semaphore lock;
struct cdev *iopwm_cdev;




static int iopwm_open(struct inode *inode,struct file *filp)
{  
if(!down_trylock(&lock))
return 0; 
else
	return -EBUSY;
}



static long iopwm_ioctl( struct file *file, unsigned int cmd, unsigned long arg)
{

		switch(cmd)
		{

			case 1:
			  	gpio_set_value(S5PV210_GPH2(0),1);
				gpio_set_value(S5PV210_GPH2(3),1);
	
				mdelay(arg);		
				gpio_set_value(S5PV210_GPH2(0),0);
				gpio_set_value(S5PV210_GPH2(3),0);
				mdelay(20-arg);
				break;
			case 3:
				gpio_set_value(S5PV210_GPH2(0),1);
				break;
			case 4:
				gpio_set_value(S5PV210_GPH2(3),1);
				mdelay(arg);
				gpio_set_value(S5PV210_GPH2(3),0);
				mdelay(20-arg);
				break;
			case 5:
				gpio_set_value(S5PV210_GPH2(3),1);
				break;
			case 6:
				gpio_set_value(S5PV210_GPH2(0),1);
				mdelay(arg);
				gpio_set_value(S5PV210_GPH2(0),0);
				mdelay(20-arg);
				break;
						
		
		
	
			
                               
		

		}
			
					
		
			return 0;
}

static int iopwm_close(struct inode *inode,struct file *filp)
{

 up(&lock);
return 0;
}

static struct file_operations iopwm_fops={
.owner=THIS_MODULE,
.open=iopwm_open,
.unlocked_ioctl=iopwm_ioctl,//从内核2.6开始需要加上unlocked_
.release=iopwm_close,
};

static struct miscdevice misc = {
.minor = MISC_DYNAMIC_MINOR,
.name = DEVICE_NAME,
.fops = &iopwm_fops,
};

static int __init iopwm_init(void)
{
	int ret;
	sema_init(&lock,1);
	ret = misc_register(&misc);
	printk (DEVICE_NAME"/tinitialized/n");

	s3c_gpio_setpull(S5PV210_GPH2(0),S3C_GPIO_PULL_UP);

        s3c_gpio_cfgpin(S5PV210_GPH2(0),S3C_GPIO_OUTPUT); //设置GPh20 和GPh21口为输出
	s3c_gpio_setpull(S5PV210_GPH2(3),S3C_GPIO_PULL_UP);

        s3c_gpio_cfgpin(S5PV210_GPH2(3),S3C_GPIO_OUTPUT); //设置GPh20 和

	return 0;
}

static void __exit iopwm_exit(void)
{
misc_deregister(&misc);
}

module_init(iopwm_init);
module_exit(iopwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gfpeak@gamil.com");
