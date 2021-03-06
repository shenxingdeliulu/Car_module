/*
**
**This is a beep driver.
**The author is Alan.
**
**
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/fd.h>
#include <linux/backlight.h>
#include <linux/err.h>
#include <linux/pwm.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>
#include <asm/uaccess.h>
#define DEVICE_NAME "pwm"  //设备名称

#define PWM_IOCTL_SET_FREQ 	1  //设置PWM的输出频率
#define PWM_IOCTL_STOP    	0  //停止PWM

#define NS_IN_1HZ		(1000000000UL)//用于设置计数器值来获取相应频率

#define BUZZER_PWM_ID		0
#define BUZZER_PWM_GPIO		S5PV210_GPD0(0)//蜂鸣器的输出端口

static struct pwm_device *pwm4buzzer;//pwm设备结构
static struct semaphore lock;//信号锁

static void pwm_stop(void)
{
	pwm_config( pwm4buzzer,0,NS_IN_1HZ / 100 );
	pwm_disable(pwm4buzzer);
}

static int gec210_pwm_open(struct inode *inode ,struct file *file)
{
	if(!down_trylock(&lock))//获取信号锁
		return 0;
	else 
		return -EBUSY;
}

static int gec210_pwm_close(struct inode *inode,struct file *file)
{
	up(&lock);//p操作，即释放信号锁
	return 0;
}
static int gec210_pwm_write(struct file *filp,const char __user *buf,size_t count,loff_t *f_pos)
{
	char mybuf[1];
//        printk("%s",mybuf);
	int p_ns = NS_IN_1HZ / 50;
	 copy_from_user(mybuf,buf,count);
	 pwm_config( pwm4buzzer,p_ns / (int)mybuf[0],p_ns );//设置周期
	 pwm_enable(pwm4buzzer);//使能pwm

        return count;

}


static long gec210_pwm_ioctl(struct file *filp , unsigned int cmd,
			unsigned long arg)
{
	switch (cmd)
	{
		case PWM_IOCTL_SET_FREQ:
			if( arg == 0 )
				return -EINVAL;
//			pwm_set_freq( arg );//设置输出频率
			break;
		case PWM_IOCTL_STOP://停止
			pwm_stop();
		default:
			break;
	}
	return 0;
}

static struct file_operations gec210_pwm_ops = {//pwm操作接口
	.owner		=	THIS_MODULE,
	.open		=	gec210_pwm_open,
	.write 		=	gec210_pwm_write,
	.release	=	gec210_pwm_close,
	.unlocked_ioctl	=	gec210_pwm_ioctl,	
};

static struct miscdevice gec210_misc_dev = {//misc设备
	.minor		=	MISC_DYNAMIC_MINOR,
	.name		=	DEVICE_NAME,
	.fops		=	&gec210_pwm_ops,
};
	
static int __init gec210_pwm_dev_init(void)
{
	int ret;
		
	ret = gpio_request(BUZZER_PWM_GPIO, DEVICE_NAME);
	if(ret)
	{
		printk("request GPIO %d for pwm failed\n",BUZZER_PWM_GPIO);
		return ret;
	}
	gpio_set_value(BUZZER_PWM_GPIO,0);
	s3c_gpio_cfgpin(BUZZER_PWM_GPIO,S3C_GPIO_OUTPUT);//设置为输出端口

	pwm4buzzer = pwm_request(BUZZER_PWM_ID,DEVICE_NAME);//请求设备资源
	if( IS_ERR(pwm4buzzer) )
	{
		printk("request pwm %d for %s failed\n",BUZZER_PWM_ID,DEVICE_NAME);
		return -ENODEV;
	}
	pwm_stop();

	s3c_gpio_cfgpin(BUZZER_PWM_GPIO,S3C_GPIO_SFN(2));
	gpio_free(BUZZER_PWM_GPIO);
	
	sema_init(&lock,1);//初始化信号锁
	ret = misc_register(&gec210_misc_dev);//注册misc设备
	
	printk(DEVICE_NAME "\tinitiazed\n");
		
	return ret;
}

static void __exit gec210_pwm_dev_exit(void)
{
	pwm_stop();  //停止pwm
		
	misc_deregister(&gec210_misc_dev); //接触misc设备
}

module_init(gec210_pwm_dev_init);
module_exit(gec210_pwm_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ALAN");
MODULE_DESCRIPTION("This is a pwm_beep driver!");

