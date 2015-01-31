/*#include <linux/module.h>    //模块化编程所需
#include <linux/kernel.h>    //printk（）需要
#include <linux/fs.h>        //包含buffer header信息
#include <linux/init.h>        //包含模块化标记函数所需
#include <linux/delay.h>    //一看就知道是延时
#include <asm/irq.h>        //中断
#include <mach/regs-gpio.h>    //GPIO相关的所有声明
#include <mach/hardware.h>    //GPIO寄存器配置的宏定义
#include <linux/gpio.h>   
#include <linux/device.h>
#include <linux/miscdevice.h> //注册设备节点
*/
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/irq.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <linux/list.h>
#include <linux/pci.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/unistd.h>




#define DEVICE_NAME     "leds_control" 

#define LED_MAJOR       231    
#define IOCTL_LED_ON    1   
#define IOCTL_LED_OFF   0

static unsigned long led_table [] = {  
    S3C2410_GPB5,
    S3C2410_GPB6,
    S3C2410_GPB7,
    S3C2410_GPB8,
};

static unsigned int led_cfg_table [] = { 
    S3C2410_GPB5_OUTP,
    S3C2410_GPB6_OUTP,
    S3C2410_GPB7_OUTP,
    S3C2410_GPB8_OUTP,
};

//应用程序对设备文件/dev/leds执行open(...)时，就会调用s3c2410_leds_open函数
static int s3c2410_leds_open(struct inode *inode, struct file *file)
{
    int i;
    for (i = 0; i < 4; i++) {
       // 设置GPIO引脚的功能：本驱动中LED所涉及的GPIO引脚设为输出功能
        s3c2410_gpio_cfgpin(led_table[i], led_cfg_table[i]);
    }
    return 0;
}

//应用程序对设备文件/dev/leds执行ioclt(...)时,就会调用s3c2410_leds_ioctl函数
static int s3c2410_leds_ioctl(
    struct inode *inode,
    struct file *file,
    unsigned int cmd,
    unsigned long arg)
{
    if (arg > 4) {
        return -EINVAL;
    }
    switch(cmd){
    case IOCTL_LED_ON:
        // 设置指定引脚的输出电平为0
        s3c2410_gpio_setpin(led_table[arg], 0);
        return 0;
    case IOCTL_LED_OFF:
        // 设置指定引脚的输出电平为1
        s3c2410_gpio_setpin(led_table[arg], 1);
        return 0;
    default:
        return -EINVAL;
    }
}

//字符设备驱动程序的核心,当应用程序操作设备文件时所调用的open、read、write等函数，最终会调用这个结构中指定的对应函数
static struct file_operations s3c2410_leds_fops = {
    .owner  =   THIS_MODULE,   
    .open   =   s3c2410_leds_open,    
    .ioctl  =   s3c2410_leds_ioctl,
};

//杂项设备节点注册，之前就是少了这一个一直没有成功，这个方法比mknod方便多了
static struct miscdevice misc = {                  
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &s3c2410_leds_fops,
};

//执行“insmod s3c2410_leds.ko”命令时就会调用这个函数
static int __init s3c2410_leds_init(void)
{
    int ret;
    int reg;
   
    ret = register_chrdev(LED_MAJOR, DEVICE_NAME, &s3c2410_leds_fops);
    if (ret < 0) {
      printk(DEVICE_NAME " can't register major number\n");
      return ret;
    }
    printk(DEVICE_NAME " initialized\n");
    reg = misc_register(&misc);
    printk(DEVICE_NAME "pls ls /dev/,checking leds_control");   
    return 0;
}


//执行”rmmod s3c24xx_leds.ko”命令时就会调用这个函数
static void __exit s3c2410_leds_exit(void)
{
        unregister_chrdev(LED_MAJOR, DEVICE_NAME);
    misc_deregister(&misc);
    printk("Now deregister\n");
}

module_init(s3c2410_leds_init);  
module_exit(s3c2410_leds_exit);
MODULE_LICENSE("GPL");
