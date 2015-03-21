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
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>

#include <linux/timer.h>
#include <linux/irq.h>
#define DEVICE_NAME "iopwm"
#define iopwm_irq IRQ_TIMER1 



struct cdev *iopwm_cdev;
unsigned int counter=0;
unsigned int pwm[]={50,50};
unsigned long tmp;
static irqreturn_t iopwm_interrupt(int irq , void *dev_id)
{
	if(irq !=IRQ_TIMER1){
	printk("bad irq % d in timer0 /n", irq);
	return -1;
}

// printk("good irq % d in timer0 /n", irq);


	counter++;
	if(counter ==pwm[0])
	{
			gpio_set_value(S5PV210_GPH2(0),0);

	}
	if(counter ==pwm[1])
	{
		gpio_set_value(S5PV210_GPH2(3),0);

	}
	if(counter ==1000)
	{
		gpio_set_value(S5PV210_GPH2(0),1);
		gpio_set_value(S5PV210_GPH2(3),1);
		counter=0;

	}

   
	return IRQ_RETVAL(IRQ_HANDLED);
}


void iopwm_timerinit(void)
{

	unsigned long tcfg0,tcfg1,tcon,cstat;
	struct clk *clk_p;
	unsigned long pclk,tcnt;
	unsigned long freq=6500;

{
		s3c_gpio_setpull(S5PV210_GPH2(0),S3C_GPIO_PULL_UP);

        s3c_gpio_cfgpin(S5PV210_GPH2(0),S3C_GPIO_OUTPUT); //设置GPh20 和GPh21口为输出
		s3c_gpio_setpull(S5PV210_GPH2(3),S3C_GPIO_PULL_UP);

        s3c_gpio_cfgpin(S5PV210_GPH2(3),S3C_GPIO_OUTPUT); //设置GPh20 和
}



tcon = __raw_readl(S3C2410_TCON);
tcfg1 = __raw_readl(S3C2410_TCFG1);
tcfg0 = __raw_readl(S3C2410_TCFG0);

//prescaler = 50
tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;
tcfg0 |= (25- 1); //预分频为50

//mux = 1/16
tcfg1 &= ~S3C2410_TCFG1_MUX1_MASK;//定时器1分割值的掩码
tcfg1 |= S3C2410_TCFG1_MUX1_DIV4;//定时器1进行4分割

__raw_writel(tcfg1, S3C2410_TCFG1);//把tcfg1的值写进分割寄存器s3c2410_TCFG1
__raw_writel(tcfg0, S3C2410_TCFG0);//


clk_p = clk_get(NULL, "pclk");
pclk = clk_get_rate(clk_p);
tcnt = (pclk/25/16)/freq;//得到定时器的输入时钟
printk("TCNT is %ld/n",tcnt);
__raw_writel(tcnt, S3C2410_TCNTB(1));//pwm脉宽的频率等于定时器的输入时钟
__raw_writel(0, S3C2410_TCMPB(1));//此处设置占空比？

 //定时器1中断使能
cstat = __raw_readl(S3C64XX_TINT_CSTAT);
//cstat &=~S3C2410_TINT_CSTAT_T0INTEN;
cstat |=0x2;
__raw_writel(cstat,S3C64XX_TINT_CSTAT);

tcon &= ~0xf1<<4;
 tcon |= 0xb<<8; //disable deadzone, auto-reload, inv-off, update TCNTB0&TCMPB0, start timer 1
__raw_writel(tcon, S3C2410_TCON);

tcon &= ~0x2<<8; //clear manual update bit
__raw_writel(tcon, S3C2410_TCON);
printk("init iopwm success!/n");
}


static int iopwm_open(struct inode *inode,struct file *filp)
{  
printk("Register iopwm success!/n");
return 0; 
}



static long iopwm_ioctl( struct file *filp, unsigned int cmd, unsigned long arg)
{

switch (cmd)
{
case 0:
pwm[0]=arg;
break;
case 1:
pwm[1]=arg;
break;
default:
return -EINVAL;
}
return 0;
}

static int iopwm_close(struct inode *inode,struct file *filp)
{
// free_irq(iopwm_irq,NULL);
return 0;
}

static struct file_operations iopwm_fops={
.owner=THIS_MODULE,
.open=iopwm_open,
.unlocked_ioctl=iopwm_ioctl,
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
ret = misc_register(&misc);
printk (DEVICE_NAME"/tinitialized/n");

iopwm_timerinit();
ret=request_irq(iopwm_irq,iopwm_interrupt,IRQ_TYPE_LEVEL_HIGH, DEVICE_NAME, NULL);
if(ret<0){
printk("Register IOPWM failed!/n");
return ret;
}
printk("Register iopwm success!/n");
return ret;
}

static void __exit iopwm_exit(void)
{
free_irq(iopwm_irq,NULL);
misc_deregister(&misc);
}

module_init(iopwm_init);
module_exit(iopwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("gfpeak@gamil.com");
